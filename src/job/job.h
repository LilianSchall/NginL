#ifndef JOB_H
#define JOB_H

#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#include "vhost/vhost.h"

enum job_type
{
    VHOST_RUN,
    PROCESS_MESSAGE,
    SEND_RESPONSE,
};

union job_func
{
    vhost_run_fn run_vhost;
    process_message_fn process_message;
    send_response_fn send_response;
};

struct job
{
    void **arg;
    enum job_type type;
    union job_func func;
};

struct job *create_job(void **arg, enum job_type type, union job_func func);

void free_job(void *data);

struct job *get_job(struct linked_list *job_queue, pthread_mutex_t *job_lock,
                    bool *running);

void execute_job(struct job *job, bool *running);

void run_job(struct job *job);

#endif /* !JOB_H */
