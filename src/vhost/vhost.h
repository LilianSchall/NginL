#ifndef VHOST_H
#define VHOST_H

#include <sys/sendfile.h>

#include "client/client.h"
#include "commons/commons.h"
#include "config/global_config/global_config.h"
#include "config/vhost_config/vhost_config.h"
#include "http/http_message/http_message.h"
#include "linked_list/linked_list.h"
#include "log/log.h"

#define HTTP_VERSION "HTTP/1.1"

struct vhost
{
    const struct global_config *global_config;
    struct vhost_config *vhost_config;
};

typedef void (*vhost_run_fn)(struct vhost *, struct linked_list *, bool *);
typedef void (*process_message_fn)(struct client *);
typedef void (*send_response_fn)(struct client *, struct vhost_config *);

#include "communication/communication.h"
#include "epoll_handler/epoll_handler.h"
#include "job/job.h"

// create the vhost struct
struct vhost *create_vhost(const struct global_config *global_config,
                           struct vhost_config *vhost_config);

// run a single vhost instance
void run_vhost(struct vhost *host, struct linked_list *job_queue,
               bool *running);

void process_message(struct client *client);
void send_response(struct client *client, struct vhost_config *config);

void free_vhost(void *vhost);

#endif /* !VHOST_H */
