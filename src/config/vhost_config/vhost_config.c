#include "vhost_config.h"

struct vhost_config *create_vhost_config(char *server_name, char *port,
                                         char *ip, char *root_dir)
{
    struct vhost_config *vhost_config = malloc(sizeof(struct vhost_config));
    if (!vhost_config)
        return NULL;
    vhost_config->reload = false;
    vhost_config->server_name = server_name;
    vhost_config->port = port;
    vhost_config->ip = ip;
    vhost_config->root_dir = root_dir;
    vhost_config->default_file = NULL;
    return vhost_config;
}

bool check_vhost_config(struct vhost_config *vc)
{
    if (!(vc->server_name && vc->port && vc->ip && vc->root_dir))
        return false;
    if (!vc->default_file)
        vc->default_file = strdup("index.html");
    return true;
}

void free_vhost_config(void *vhost_config)
{
    if (!vhost_config)
        return;
    struct vhost_config *vhc = vhost_config;
    if (vhc->server_name)
    {
        free(vhc->server_name);
        vhc->server_name = NULL;
    }
    if (vhc->port)
    {
        free(vhc->port);
        vhc->port = NULL;
    }
    if (vhc->ip)
    {
        free(vhc->ip);
        vhc->ip = NULL;
    }
    if (vhc->root_dir)
    {
        free(vhc->root_dir);
        vhc->root_dir = NULL;
    }
    if (vhc->default_file)
    {
        free(vhc->default_file);
        vhc->default_file = NULL;
    }
    free(vhost_config);
}
