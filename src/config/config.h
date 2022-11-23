#ifndef CONFIG_H
#define CONFIG_H

#include "config/global_config/global_config.h"
#include "config/vhost_config/vhost_config.h"
#include "config_parser.h"
#include "linked_list/linked_list.h"
#include "log/log.h"

extern struct config *config_static;

struct config
{
    char *path;
    struct global_config *global_config;
    struct linked_list *vhost_list;
};

struct config *create_config(struct global_config *global_config,
                             struct linked_list *vhost_list, char *path);

void reload_config();
void free_config(struct config *config);

#endif /* !CONFIG_H */
