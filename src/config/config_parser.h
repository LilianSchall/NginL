#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "linked_list/linked_list.h"

// this function will verify and parse the given file in arg
// returns NULL if the config_file is badly formatted
// This is used to make the --dry-run option
struct config *parse_config_file(char *file);

#endif /* !CONFIG_PARSER_H */
