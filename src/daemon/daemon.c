#include "daemon.h"

struct thread_pool *thread_pool = NULL;

void sig_handler(int sig)
{
    if (sig == SIGINT)
    {
        stop_pool(thread_pool);
        thread_pool = NULL;
    }
    if (sig == SIGUSR1)
    {
        reload_config();
    }
}

void run_server(struct config *config)
{
    config_static = config;

    init_log(config->global_config->log_file, config->global_config->log);

    size_t nb_vhost = list_size(config->vhost_list);
    struct thread_pool *pool = create_pool(nb_vhost * COEFF_THREADS);

    struct linked_node *node = NULL;

    void ***args = calloc(nb_vhost, sizeof(void **));
    size_t i = 0;

    union job_func fun;
    fun.run_vhost = run_vhost;

    foreach(node, config->vhost_list)
    {
        void **arg = malloc(2 * sizeof(void *));
        struct vhost_config *vhost_config = node->data;

        struct vhost *host = create_vhost(config->global_config, vhost_config);

        arg[0] = host;
        arg[1] = pool->job_queue;

        struct job *job = create_job(arg, VHOST_RUN, fun);
        pool->job_queue = list_append(pool->job_queue, job);
        args[i++] = arg;
    }

    thread_pool = pool;
    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGPIPE, SIG_IGN);
    wait_pool(pool);
    print_log("Pool stopped\n");
    fflush(stdout);
    for (size_t j = 0; j < i; j++)
    {
        free(args[j]);
    }
    free(args);
    free_pool(pool);
    close_log();
}

int start_daemon(struct config *config)
{
    FILE *pid_file = fopen(config->global_config->pid_file, "r");
    int pid;
    if (pid_file)
    {
        size_t n = 0;
        char *line = NULL;
        ssize_t numchar = getline(&line, &n, pid_file);
        if (numchar == -1)
        {
            fprintf(stderr, "Error: the file '%s' is empty",
                    config->global_config->pid_file);
            free(line);
            return 1;
        }
        pid = atoi(line);
        free(line);
        if (kill(pid, 0) == 0)
        {
            return 1;
        }
    }
    pid_file = fopen(config->global_config->pid_file, "w");
    if (!pid_file)
    {
        fprintf(stderr, "Error: could not open file for writting: '%s'\n",
                config->global_config->pid_file);
        return 1;
    }
    pid = fork();
    if (pid)
    {
        fprintf(pid_file, "%d\n", pid);
        fclose(pid_file);
    }
    else
    {
        run_server(config);
        exit(0);
    }
    return 0;
}

void restart_daemon(struct config *config)
{
    stop_daemon(config);
    start_daemon(config);
}

void reload_daemon(struct config *config)
{
    FILE *pid_file = fopen(config->global_config->pid_file, "r");
    if (!pid_file)
        return;
    size_t n = 0;
    char *line = NULL;
    ssize_t numchar = getline(&line, &n, pid_file);
    if (numchar == -1)
    {
        fprintf(stderr, "Error: the file '%s' is empty",
                config->global_config->pid_file);
        free(line);
        return;
    }
    int pid = atoi(line);
    free(line);
    if (kill(pid, 0) == 0)
    {
        kill(pid, SIGUSR1);
    }
}

void stop_daemon(struct config *config)
{
    FILE *pid_file = fopen(config->global_config->pid_file, "r");
    if (!pid_file)
        return;
    size_t n = 0;
    char *line = NULL;
    ssize_t numchar = getline(&line, &n, pid_file);
    if (numchar == -1)
    {
        fprintf(stderr, "Error: the file '%s' is empty",
                config->global_config->pid_file);
        free(line);
        return;
    }
    int pid = atoi(line);
    free(line);
    if (kill(pid, 0) == 0)
    {
        kill(pid, SIGINT);
        size_t msec = 10;
        struct timespec ts;
        ts.tv_sec = msec / 1000;
        ts.tv_nsec = (msec % 1000) * 1000000;

        while (kill(pid, 0) == 0)
            nanosleep(&ts, &ts);
    }
}
