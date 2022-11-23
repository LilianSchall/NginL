#include "vhost.h"

#include <stdio.h>

#define MAX_EVENTS 10
#define NB_MAX_CLIENTS 4096

struct vhost *create_vhost(const struct global_config *global_config,
                           struct vhost_config *vhost_config)
{
    struct vhost *host = malloc(sizeof(struct vhost));

    if (host == NULL)
        return NULL;

    host->global_config = global_config;
    host->vhost_config = vhost_config;

    return host;
}

void run_vhost(struct vhost *host, struct linked_list *job_queue, bool *running)
{
    struct communication *com =
        create_communication(host->vhost_config->ip, host->vhost_config->port);

    if (com == NULL) // crash while creating com
    {
        print_log("Could not create comms\n");
        free_vhost(host);
        return;
    }

    struct epoll_handler *ep = create_epoll_handler(com);

    if (ep == NULL)
    {
        print_log("Could not create epoll_handler\n");
        free_vhost(host);
        stop_communication(com);
        free_communication(com);
        return;
    }

    while (*running)
    {
        if (!use_epoll_handler(ep, com, host->vhost_config, job_queue))
        {
            print_log("Crash, proper exit...\n");
            free_vhost(host);
            free_epoll_handler(ep, com);
            stop_communication(com);
            free_communication(com);
            return;
        }
        close_finished_clients(ep, com);
        if (host->vhost_config->reload)
        {
            free_epoll_handler(ep, com);
            stop_communication(com);
            free_communication(com);
            com = create_communication(host->vhost_config->ip,
                                       host->vhost_config->port);
            ep = create_epoll_handler(com);
            host->vhost_config->reload = false;
        }
        // trigger_write_client(com, job_queue, host->vhost_config);
    }
    print_log("Proper exit vhost !\n");
    free_vhost(host);
    free_epoll_handler(ep, com);
    stop_communication(com);
    free_communication(com);
}

void free_vhost(void *vhost)
{
    struct vhost *host = vhost;
    if (host == NULL)
        return;

    free_vhost_config(host->vhost_config);
    free(host);
}
