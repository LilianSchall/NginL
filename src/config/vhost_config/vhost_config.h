#ifndef VHOST_CONFIG_H
#define VHOST_CONFIG_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vhost_config
{
    bool reload;
    // MANDATORY
    char *server_name;
    // MANDATORY
    char *port;
    // MANDATORY
    char *ip;
    // MANDATORY
    char *root_dir;
    // OPTIONAL
    char *default_file; // default: index.html
};

struct vhost_config *create_vhost_config(char *server_name, char *port,
                                         char *ip, char *root_dir);

bool check_vhost_config(struct vhost_config *vhost_config);

void free_vhost_config(void *vhost_config);

#endif /* !VHOST_CONFIG_H */
