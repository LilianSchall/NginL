#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct global_config
{
    // MANDATORY
    char *pid_file;
    // OPTIONAL
    char *log_file; // default if null : stdout
    // OPTIONAL
    bool log; // default = true
};

struct global_config *create_global_config(char *pid_file, char *log_file,
                                           bool log);

bool check_global_config(struct global_config *gc);

void free_global_config(struct global_config *global_config);

#endif /* !GLOBAL_CONFIG_H */
