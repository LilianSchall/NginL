#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#include "job/job.h"
#include "linked_list/linked_list.h"

#define foreach(Variable, List)                                                \
    for (Variable = List->head; Variable != NULL; Variable = Variable->next)

struct thread_pool
{
    // will store data of type struct thread
    struct linked_list *threads;

    // a queue of jobs
    struct linked_list *job_queue;
    // number of threads
    size_t size;

    pthread_mutex_t job_lock;
};

struct thread
{
    pthread_t tid; // the pid of the thread
    bool running; // attribute that tells the thread to continue running
    struct thread_pool *pool;
};

struct thread *create_thread(void);

void free_thread(void *data);

// create a thread pool of a given size
struct thread_pool *create_pool(size_t size);

// this is a function used only by thread
// wait until it gets a new job with the function
// get_job() and exec the job
// it will loop until running is set to false
void *start_worker(void *vargp);

// set attribute running of every pool to false
// and wait until every thread is finished
// return 0 if everything went well, 1 if an error occured
int stop_pool(struct thread_pool *pool);

void wait_pool(struct thread_pool *pool);

// free thread pool
void free_pool(struct thread_pool *pool);

#endif /* !THREAD_POOL_H */
