#include "job.h"

struct job *create_job(void **arg, enum job_type type, union job_func func)
{
    struct job *job = malloc(sizeof(struct job));

    if (job == NULL)
        return NULL;

    job->arg = arg;
    job->type = type;
    job->func = func;

    return job;
}

void free_job(void *data)
{
    if (data == NULL)
        return;

    free(data);
}

struct job *get_job(struct linked_list *job_queue, pthread_mutex_t *job_lock,
                    bool *running)
{
    if (job_queue == NULL)
        return NULL;
    pthread_mutex_lock(job_lock);
    struct job *job = NULL;

    size_t msec = 10;

    struct timespec ts;

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do
    {
        job = list_head(job_queue);
        nanosleep(&ts, &ts);
    } while (job == NULL && (*running));

    if (job == NULL)
    {
        pthread_mutex_unlock(job_lock);
        return NULL;
    }

    job_queue = list_pop(job_queue);
    pthread_mutex_unlock(job_lock);

    return job;
}

void execute_job(struct job *job, bool *running)
{
    if (job == NULL)
    {
        return;
    }

    print_log("Execute job\n");
    switch (job->type)
    {
    case VHOST_RUN: {
        vhost_run_fn fun = job->func.run_vhost;
        fun(job->arg[0], job->arg[1], running);
    }
    break;
    case PROCESS_MESSAGE: {
        process_message_fn fun = job->func.process_message;
        fun(job->arg[0]);
    }
    break;
    case SEND_RESPONSE: {
        send_response_fn fun = job->func.send_response;
        fun(job->arg[0], job->arg[1]);
    }
    break;
    }

    if (job->type != VHOST_RUN)
        free(job->arg);
}
