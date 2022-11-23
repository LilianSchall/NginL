#include "create_specific_job.h"

struct job *create_process_incoming_message_job(struct client *client)
{
    void **arg = calloc(1, sizeof(void *));
    arg[0] = client;
    union job_func func;

    func.process_message = process_message;

    return create_job(arg, PROCESS_MESSAGE, func);
}

struct job *create_send_response_job(struct client *client,
                                     struct vhost_config *config)
{
    void **arg = calloc(2, sizeof(void *));
    arg[0] = client;
    arg[1] = config;

    union job_func func;

    func.send_response = send_response;

    return create_job(arg, SEND_RESPONSE, func);
}
