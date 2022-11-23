#include "thread_pool.h"

#include <stdio.h>

struct thread *create_thread(void)
{
    struct thread *thread = malloc(sizeof(struct thread));

    if (thread == NULL)
        return NULL;

    thread->tid = 0;
    thread->running = true;
    thread->pool = NULL;

    return thread;
}

void free_thread(void *data)
{
    if (data == NULL)
        return;

    free(data);
}

struct thread_pool *create_pool(size_t size)
{
    struct thread_pool *pool = malloc(sizeof(struct thread_pool));

    if (pool == NULL)
        return NULL;

    struct linked_list *threads = list_create();
    if (threads == NULL)
    {
        free(pool);
        return NULL;
    }

    struct linked_list *job_queue = list_create();
    if (job_queue == NULL)
    {
        free(pool);
        list_free(threads);
        return NULL;
    }

    pool->size = size;
    pool->threads = threads;
    pool->job_queue = job_queue;
    pthread_mutex_init(&pool->job_lock, NULL);
    for (size_t i = 0; i < size; i++)
    {
        struct thread *thread = create_thread();

        if (thread == NULL)
        {
            list_deep_free(threads, free_thread);
            list_free(job_queue);
            free(pool);
            return NULL;
        }

        pthread_t tid;
        thread->pool = pool;

        pthread_create(&tid, NULL, start_worker, thread);
        thread->tid = tid;
        threads = list_append(threads, thread);
    }

    return pool;
}

void *start_worker(void *vargp)
{
    struct thread *self = vargp;
    struct thread_pool *pool = self->pool;

    while (self->running)
    {
        struct job *job =
            get_job(pool->job_queue, &pool->job_lock, &self->running);

        print_log("Got job for thread %p\n", vargp);
        if (job == NULL)
            return NULL;
        execute_job(job, &self->running);
        free_job(job);
        print_log("Thread %p terminated its job\n", vargp);
    }

    return NULL;
}

int stop_pool(struct thread_pool *pool)
{
    if (pool == NULL)
        return 1;
    struct linked_node *node = NULL;
    foreach(node, pool->threads)
    {
        struct thread *thread = node->data;
        if (thread == NULL)
            return 1;

        thread->running = false;
        // no need to wait the end of worker with pthread_join(2)
    }
    return 0;
}

void wait_pool(struct thread_pool *pool)
{
    struct linked_node *node = NULL;
    foreach(node, pool->threads)
    {
        struct thread *thread = node->data;

        pthread_join(thread->tid, NULL);
    }
}

void free_pool(struct thread_pool *pool)
{
    if (pool == NULL)
        return;

    list_deep_free(pool->threads, free_thread);
    list_deep_free(pool->job_queue, free_job);

    pthread_mutex_destroy(&pool->job_lock);

    free(pool);
}
