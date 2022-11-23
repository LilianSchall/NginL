#include "http_method.h"

enum http_method str_to_method(char *str)
{
    if (str == NULL)
        return METHOD_ERROR;

    if (!strcmp(str, "GET"))
        return GET;
    if (!strcmp(str, "HEAD"))
        return HEAD;

    return METHOD_ERROR;
}
