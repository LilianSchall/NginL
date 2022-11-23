#ifndef HTTP_METHOD_H
#define HTTP_METHOD_H

#include <stddef.h>
#include <string.h>

enum http_method
{
    GET,
    HEAD,
    METHOD_ERROR,
};

enum http_method str_to_method(char *str);

#endif /* !HTTP_METHOD_H */
