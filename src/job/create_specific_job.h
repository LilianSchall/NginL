#ifndef CREATE_SPECIFIC_JOB_H
#define CREATE_SPECIFIC_JOB_H

#include "client/client.h"
#include "job.h"

struct job *create_process_incoming_message_job(struct client *client);
struct job *create_send_response_job(struct client *client,
                                     struct vhost_config *config);

#endif /* !CREATE_SPECIFIC_JOB_H */
