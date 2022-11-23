#include "starting_option.h"

enum starting_option parse_args(int argc, char **argv, char **config_file)
{
    enum starting_option return_value = DEFAULT;
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-a"))
        {
            if (return_value)
                return ERROR;
            i++;
            if (i >= argc)
                return ERROR;
            if (!strcmp(argv[i], "start"))
                return_value = START;
            else if (!strcmp(argv[i], "stop"))
                return_value = STOP;
            else if (!strcmp(argv[i], "reload"))
                return_value = RELOAD;
            else if (!strcmp(argv[i], "restart"))
                return_value = RESTART;
            else
                return ERROR;
        }
        else if (!strcmp(argv[i], "--dry-run"))
        {
            if (return_value)
                return ERROR;
            return_value = DRY_RUN;
        }
        else if (*config_file)
            return ERROR;
        else
            *config_file = strdup(argv[i]);
    }
    return return_value;
}

int handle_option(enum starting_option option, char *config_file)
{
    struct config *config = parse_config_file(config_file);

    if (config == NULL)
        return 2;

    // config is correct

    int retval = 0;

    switch (option)
    {
    case DEFAULT:
        run_server(config);
        free(config->path);
        free_global_config(config->global_config);
        list_free(config->vhost_list);
        free(config);
        return 0;
        break;
    case START:
        retval = start_daemon(config);
        break;
    case RELOAD:
        reload_daemon(config);
        break;
    case RESTART:
        restart_daemon(config);
        break;
    case STOP:
        stop_daemon(config);
        break;
    default:
        break;
    }

    free_config(config);

    return retval;
}
