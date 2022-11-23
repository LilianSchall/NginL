#include "config_parser.h"

bool set_field(char **field, char *token)
{
    if (!token || *field)
        return false;
    *field = strdup(token);
    return true;
}

bool parse_line_global(char *line, struct global_config *global_config)
{
    char *token = strtok(line, " =\n");
    if (!strcmp(token, "log_file"))
    {
        token = strtok(NULL, " =\n");
        if (!set_field(&(global_config->log_file), token))
            return false;
    }
    else if (!strcmp(token, "pid_file"))
    {
        token = strtok(NULL, " =\n");
        if (!set_field(&(global_config->pid_file), token))
            return false;
    }
    else if (!strcmp(token, "log"))
    {
        token = strtok(NULL, " =\n");
        if (!strcmp(token, "false"))
            global_config->log = false;
        else if (strcmp(token, "true"))
            return false;
    }
    else
        return false;
    token = strtok(NULL, " \n");
    if (token)
        return false;
    return true;
}

struct global_config *parse_global(FILE *f)
{
    struct global_config *global_config =
        create_global_config(NULL, NULL, true);
    size_t n = 0;
    char *line = NULL;
    ssize_t numchar = getline(&line, &n, f);
    if (strcmp(line, "[global]\n"))
    {
        free_global_config(global_config);
        free(line);
        return NULL;
    }
    free(line);
    line = NULL;
    numchar = getline(&line, &n, f);
    while (numchar != -1 && strcmp(line, "\n"))
    {
        if (!(parse_line_global(line, global_config)))
        {
            free_global_config(global_config);
            return NULL;
        }
        free(line);
        line = NULL;
        numchar = getline(&line, &n, f);
    }
    free(line);
    if (!check_global_config(global_config))
    {
        free_global_config(global_config);
        return NULL;
    }
    return global_config;
}

bool parse_line_vhost(char *line, struct vhost_config *vhost_config)
{
    char *token = strtok(line, " =\n");
    if (!strcmp(token, "server_name"))
    {
        token = strtok(NULL, " =\n");
        if (!set_field(&(vhost_config->server_name), token))
            return false;
    }
    else if (!strcmp(token, "port"))
    {
        token = strtok(NULL, " =\n");
        if (!set_field(&(vhost_config->port), token))
            return false;
    }
    else if (!strcmp(token, "ip"))
    {
        token = strtok(NULL, " =\n");
        if (!set_field(&(vhost_config->ip), token))
            return false;
    }
    else if (!strcmp(token, "root_dir"))
    {
        token = strtok(NULL, " =\n");
        if (!set_field(&(vhost_config->root_dir), token))
            return false;
    }
    else if (!strcmp(token, "default_file"))
    {
        token = strtok(NULL, " =\n");
        if (!set_field(&(vhost_config->default_file), token))
            return false;
    }
    else
        return false;
    token = strtok(NULL, " \n");
    if (token)
        return false;
    return true;
}

struct vhost_config *parse_vhost(FILE *f, bool *err)
{
    struct vhost_config *vhost_config =
        create_vhost_config(NULL, NULL, NULL, NULL);
    size_t n = 0;
    char *line = NULL;
    ssize_t numchar = getline(&line, &n, f);
    if (strcmp(line, "[[vhosts]]\n"))
    {
        free_vhost_config(vhost_config);
        free(line);
        *err = true;
        return NULL;
    }
    free(line);
    line = NULL;
    numchar = getline(&line, &n, f);
    while (numchar != -1 && strcmp(line, "\n"))
    {
        if (!(parse_line_vhost(line, vhost_config)))
        {
            free_vhost_config(vhost_config);
            *err = true;
            free(line);
            return NULL;
        }
        free(line);
        line = NULL;
        numchar = getline(&line, &n, f);
    }
    free(line);
    if (!check_vhost_config(vhost_config))
    {
        *err = true;
        free_vhost_config(vhost_config);
        return NULL;
    }
    return vhost_config;
}

struct config *parse_config_file(char *file)
{
    FILE *f = fopen(file, "r");
    if (!f)
        return NULL;
    struct global_config *global_config = parse_global(f);
    bool err = false;
    struct linked_list *vhost_list = list_create();
    struct vhost_config *vhost_config = parse_vhost(f, &err);
    if (!vhost_config || err)
    {
        free_vhost_config(vhost_config);
        vhost_config = NULL;
    }
    int c = fgetc(f);
    if (c != EOF)
        ungetc(c, f);
    while (!feof(f) && vhost_config && !err)
    {
        vhost_list = list_append(vhost_list, vhost_config);
        vhost_config = parse_vhost(f, &err);
        int c = fgetc(f);
        if (c != EOF)
            ungetc(c, f);
    }
    if (vhost_config)
        vhost_list = list_append(vhost_list, vhost_config);
    if (err)
    {
        free_global_config(global_config);
        global_config = NULL;
        list_deep_free(vhost_list, free_vhost_config);
        vhost_list = NULL;
    }
    return create_config(global_config, vhost_list, file);
}
