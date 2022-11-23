#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stdlib.h>
#include <string.h>

#include "commons/commons.h"
#include "http/delimiter.h"
#include "http/http_method/http_method.h"

// this is the first line of a http_message
// it is of the form:
// [METHOD] SP [REQUEST_TARGET] SP [HTTP-VERSION] CRLF
struct http_request
{
    enum http_method method;
    char *target;
    char *http_version;
};

struct http_request *create_http_request(enum http_method method, char *target,
                                         char *http_version);

void free_http_request(void *data);

// this function parse the given line
// (expected to be the first line of a http message)
// into a request struct
struct http_request *parse_request(char *line);

#endif /* !HTTP_REQUEST_H */
