#include "http_status.h"

struct http_status *create_http_status(char *http_version,
                                       enum status_code code, char *reason)
{
    if (!http_version || code == CODE_ERROR || !reason)
        return NULL;

    struct http_status *status = malloc(sizeof(struct http_status));

    if (status == NULL)
        return NULL;

    status->http_version = http_version;
    status->code = code;
    status->reason = reason;

    return status;
}

void free_http_status(struct http_status *http_status)
{
    if (http_status == NULL)
        return;

    free(http_status);
}
