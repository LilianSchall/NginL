#include "global_config.h"

struct global_config *create_global_config(char *pid_file, char *log_file,
                                           bool log)
{
    struct global_config *global_config = malloc(sizeof(struct global_config));
    global_config->pid_file = pid_file;
    global_config->log_file = log_file;
    global_config->log = log;
    return global_config;
}

bool check_global_config(struct global_config *gc)
{
    return gc->pid_file;
}

void free_global_config(struct global_config *global_config)
{
    if (!global_config)
        return;
    if (global_config->pid_file)
        free(global_config->pid_file);
    if (global_config->log_file)
        free(global_config->log_file);
    free(global_config);
}
