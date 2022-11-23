#ifndef HTTP_STATUS_H
#define HTTP_STATUS_H

#include <stdlib.h>

enum status_code
{
    OK = 200,
    BAD_REQUEST = 400,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    NOT_ALLOWED = 405,
    NOT_SUPPORTED = 505,
    CODE_ERROR,
};

#define CREATE_REASON                                                          \
    char *reasons[] = {                                                        \
        [OK] = "OK",                                                           \
        [FORBIDDEN] = "Forbidden",                                             \
        [NOT_FOUND] = "Not Found",                                             \
        [NOT_ALLOWED] = "Method Not Allowed",                                  \
        [NOT_SUPPORTED] = "HTTP Version Not Supported",                        \
        [BAD_REQUEST] = "Bad Request",                                         \
    }

struct http_status
{
    char *http_version;
    enum status_code code;
    char *reason;
};

struct http_status *create_http_status(char *http_version,
                                       enum status_code code, char *reason);

void free_http_status(struct http_status *http_status);

#endif /* !HTTP_STATUS_H */
