#include "http_request.h"

#include <stdio.h>

#define NB_TOKENS_REQUEST 3

struct http_request *create_http_request(enum http_method method, char *target,
                                         char *http_version)
{
    if (!target || !http_version)
        return NULL;

    struct http_request *request = malloc(sizeof(struct http_request));

    if (request == NULL)
        return NULL;

    request->method = method;
    request->target = target;
    request->http_version = http_version;

    return request;
}

void free_http_request(void *data)
{
    if (data == NULL)
        return;

    struct http_request *request = data;

    free(request->target);
    free(request->http_version);

    free(request);
}

struct http_request *parse_request(char *line)
{
    if (line == NULL)
        return NULL;

    char *tokens[NB_TOKENS_REQUEST] = { NULL };
    char *lineptr = NULL;
    for (size_t i = 0; i < NB_TOKENS_REQUEST; i++)
    {
        char *token =
            strtok_r(i == 0 ? line : NULL,
                     i == NB_TOKENS_REQUEST - 1 ? CRLF : SP, &lineptr);

        if (token == NULL)
            return NULL;

        if (strstr(token, i == NB_TOKENS_REQUEST - 1 ? SP : CRLF) != NULL)
            return NULL;

        tokens[i] = strdup(token);
    }

    enum http_method method = str_to_method(tokens[0]);

    struct http_request *request =
        create_http_request(method, tokens[1], tokens[2]);

    free_array_content(tokens, request == NULL ? NB_TOKENS_REQUEST : 1,
                       sizeof(char *));
    return request;
}
