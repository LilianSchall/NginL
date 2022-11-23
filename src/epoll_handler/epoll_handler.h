#ifndef EPOLL_HANDLER_H
#define EPOLL_HANDLER_H

#define MAX_EVENTS 10

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "commons/commons.h"
#include "communication/communication.h"
#include "config/vhost_config/vhost_config.h"
#include "job/create_specific_job.h"

struct epoll_handler
{
    int fd;
    struct epoll_event events[MAX_EVENTS];
    pthread_mutex_t lock;
};

struct epoll_handler *create_epoll_handler(struct communication *com);

bool use_epoll_handler(struct epoll_handler *ep, struct communication *com,
                       struct vhost_config *config,
                       struct linked_list *job_queue);

void close_finished_clients(struct epoll_handler *ep,
                            struct communication *com);

void free_epoll_handler(struct epoll_handler *ep, struct communication *com);

#endif /* !EPOLL_HANDLER_H */
