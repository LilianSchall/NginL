#include "config.h"

struct config *config_static;

struct config *create_config(struct global_config *global_config,
                             struct linked_list *vhost_list, char *path)
{
    if (!global_config || !vhost_list || !vhost_list->head)
    {
        free_global_config(global_config);
        list_deep_free(vhost_list, free_vhost_config);
        return NULL;
    }
    struct config *cfg = malloc(sizeof(struct config));
    if (!cfg)
        return NULL;
    cfg->path = strdup(path);
    cfg->global_config = global_config;
    cfg->vhost_list = vhost_list;
    return cfg;
}

bool update_field(char **field1, char **field2)
{
    if (!*field1)
    {
        if (*field2)
        {
            *field1 = strdup(*field2);
            return true;
        }
    }
    else if (field2 && strcmp(*field1, *field2))
    {
        free(*field1);
        *field1 = strdup(*field2);
        return true;
    }
    else if (!*field2)
    {
        free(*field1);
        *field1 = NULL;
        return true;
    }
    return false;
}

bool compare_vhost(struct vhost_config *vhc1, struct vhost_config *vhc2)
{
    bool return_val = false;
    if (update_field(&(vhc1->server_name), &(vhc2->server_name)))
        return_val = true;
    if (update_field(&(vhc1->port), &(vhc2->port)))
        return_val = true;
    if (update_field(&(vhc1->ip), &(vhc2->ip)))
        return_val = true;
    if (update_field(&(vhc1->root_dir), &(vhc2->root_dir)))
        return_val = true;
    if (update_field(&(vhc1->default_file), &(vhc2->default_file)))
        return_val = true;
    return return_val;
}

void reload_config()
{
    struct config *new_config = parse_config_file(config_static->path);

    if (new_config == NULL)
        return;

    bool change_log = false;
    if (config_static->global_config->log != new_config->global_config->log)
    {
        change_log = true;
        config_static->global_config->log = new_config->global_config->log;
    }
    if (update_field(&(config_static->global_config->log_file),
                     &(new_config->global_config->log_file))
        || change_log)
    {
        close_log();
        init_log(config_static->global_config->log_file,
                 config_static->global_config->log);
    }
    struct linked_list *vhost_list = config_static->vhost_list;
    struct linked_list *new_vhost_list = new_config->vhost_list;
    struct linked_node *vhost_node = vhost_list->head;
    struct linked_node *new_vhost_node = new_vhost_list->head;
    int i = 0;
    while (vhost_node && new_vhost_node)
    {
        struct vhost_config *vhc = vhost_node->data;
        struct vhost_config *new_vhc = new_vhost_node->data;
        if (compare_vhost(vhc, new_vhc))
        {
            vhc->reload = true;
            print_log("Reloading vhost number %d\n", i++);
        }
        vhost_node = vhost_node->next;
        new_vhost_node = new_vhost_node->next;
    }
    free_config(new_config);
}

void free_config(struct config *config)
{
    free(config->path);
    free_global_config(config->global_config);
    list_deep_free(config->vhost_list, free_vhost_config);
    free(config);
}
