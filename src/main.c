#include <stdio.h>

#include "starting_option/starting_option.h"

int main(int argc, char **argv)
{
    char *config_file = NULL;
    enum starting_option option = parse_args(argc, argv, &config_file);

    if (!config_file || option == ERROR)
        return 1;

    return handle_option(option, config_file);
}
