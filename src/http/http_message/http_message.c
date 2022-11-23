#include "http_message.h"

union http_line create_http_line(struct http_request *request,
                                 struct http_status *status)
{
    union http_line line;

    if (request != NULL)
        line.request = request;
    else
        line.status = status;
    return line;
}

struct start_line create_start_line(union http_line line, enum line_type type)
{
    struct start_line start_line = {
        .type = type,
        .line = line,
    };

    return start_line;
}

void free_start_line(struct start_line start_line)
{
    switch (start_line.type)
    {
    case REQUEST:
        free_http_request(start_line.line.request);
        break;
    case STATUS:
        free_http_status(start_line.line.status);
        break;
    }
}

struct http_message *create_http_message(struct start_line start_line,
                                         struct hash_map *headers, char *body)
{
    if (headers == NULL || body == NULL)
        return NULL;

    struct http_message *message = malloc(sizeof(struct http_message));

    if (message == NULL)
        return NULL;
    message->start_line = start_line;
    message->headers = headers;
    message->body = body;
    message->file_to_serve = -1;
    message->file_size = -1;
    return message;
}

void free_http_message(void *data)
{
    if (data == NULL)
        return;

    struct http_message *message = data;

    hash_map_free(message->headers, message->start_line.type != STATUS);

    if (message->start_line.type == REQUEST)
        free(message->body);

    free_start_line(message->start_line);

    free(message);
}

void print_message(struct http_message *message)
{
    if (message == NULL)
    {
        print_log("Message is null\n");
        return;
    }

    if (message->start_line.type == REQUEST)
    {
        enum http_method method = message->start_line.line.request->method;
        print_log("method: %s\n",
                  method == GET        ? "GET"
                      : method == HEAD ? "HEAD"
                                       : "ERROR");
        print_log("target: %s\n", message->start_line.line.request->target);
        print_log("version: %s\n",
                  message->start_line.line.request->http_version);
    }
    else
    {
        struct http_status *status = message->start_line.line.status;
        print_log("version: %s\n", status->http_version);
        print_log("code: %d\n", status->code);
        print_log("reason: %s\n", status->reason);
    }
    hash_map_dump(stdout, message->headers);
}

static bool parse_header(struct hash_map *headers, char *line)
{
    char *tokens[NB_TOKENS_HEADER] = { NULL };
    char *lineptr = NULL;
    for (size_t i = 0; i < NB_TOKENS_HEADER; i++)
    {
        char *token =
            strtok_r(i == 0 ? line : NULL,
                     i == NB_TOKENS_HEADER - 1 ? CRLF : ":", &lineptr);

        if (token == NULL)
            return false;

        if (i == NB_TOKENS_HEADER - 1)
            token = strtrim(token);

        if (strstr(token, CRLF) != NULL)
            return false;
        tokens[i] = strdup(token);
    }
    bool operate = false;
    if (tokens[0][strlen(tokens[0]) - 1] != ' ')
        operate = hash_map_insert(headers, tokens[0], tokens[1], NULL);

    if (!operate)
    {
        free(tokens[0]);
        free(tokens[1]);
    }
    return operate;
}

static char *parse_body(FILE *stream)
{
    ssize_t capacity = BODY_BUFFER_SIZE;

    char buffer[BODY_BUFFER_SIZE] = { 0 };

    ssize_t i = 0;
    ssize_t size_read = 0;

    char *body = calloc(capacity, sizeof(char));

    while ((size_read = fread(buffer, sizeof(char), BODY_BUFFER_SIZE, stream))
           > 0)
    {
        if (i + size_read >= capacity)
        {
            body = realloc(body, capacity * 2 * sizeof(char));
            capacity *= 2;
        }
        memcpy(body + i, buffer, size_read);
        i += size_read;
    }

    return body;
}

static struct http_message *
wrapper_create_message_in(struct http_request *request,
                          struct hash_map *headers, char *body,
                          enum status_code *code)
{
    struct http_message *message = create_http_message(
        create_start_line(create_http_line(request, NULL), REQUEST), headers,
        body);

    if (!message)
    {
        *code = BAD_REQUEST;
        free(body);
        hash_map_free(headers, true);
        free_http_request(request);
        return NULL;
    }

    return message;
}

struct http_message *parse_message(FILE *stream, enum status_code *code)
{
    if (stream == NULL)
    {
        *code = BAD_REQUEST;
        return NULL;
    }

    struct http_request *request = NULL;
    size_t n = 0;
    char *line = NULL;

    if (getline(&line, &n, stream) < 1)
    {
        *code = OK;
        print_log("get line error\n");
        return NULL;
    }

    if ((request = parse_request(line)) == NULL)
    {
        free(line);
        *code = BAD_REQUEST;
        return NULL;
    }
    free(line);
    line = NULL;
    bool parsing_headers = true;
    struct hash_map *headers = hash_map_init(STD_NB_HEADERS);

    while (parsing_headers)
    {
        if (getline(&line, &n, stream) < 1 || !strcmp(line, CRLF)
            || !strcmp(line, "\n"))
            parsing_headers = false;
        else if (!parse_header(headers, line))
        {
            *code = BAD_REQUEST;
            free(line);
            hash_map_free(headers, true);
            free_http_request(request);
            return NULL;
        }

        free(line);
        line = NULL;
    }

    free(line);
    // we have arrived at the body
    // now we have to get the entire string of the body

    char *body = parse_body(stream);

    return wrapper_create_message_in(request, headers, body, code);
}

struct http_request *get_request(struct http_message *message)
{
    if (message == NULL)
        return NULL;

    return message->start_line.line.request;
}

struct http_status *get_status(struct http_message *message)
{
    if (message == NULL)
        return NULL;

    return message->start_line.line.status;
}
