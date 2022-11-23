#ifndef STARTING_OPTION_H
#define STARTING_OPTION_H

#include <string.h>

#include "config/config.h"
#include "config/config_parser.h"
#include "daemon/daemon.h"
#include "thread_pool/thread_pool.h"
#include "vhost/vhost.h"

enum starting_option
{
    DEFAULT,
    ERROR,
    START,
    STOP,
    RELOAD,
    RESTART,
    DRY_RUN,
};

enum starting_option parse_args(int argc, char **argv, char **config_file);

int handle_option(enum starting_option, char *config_file);

#endif /* !STARTING_OPTION_H */
