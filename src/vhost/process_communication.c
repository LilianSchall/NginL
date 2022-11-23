#include "vhost.h"

static void empty_socket_fd(struct client *client)
{
    char buf[1024];

    while (read(client->fd, buf, 1024) > 0)
        continue;

    while (fread(buf, sizeof(char), 1024, client->stream) > 0)
        continue;
}

void process_message(struct client *client)
{
    if (client == NULL)
    {
        print_log("Given Client to process_message is null\n");
        return;
    }

    enum status_code code = OK;
    struct http_message *message = parse_message(client->stream, &code);

    if (message == NULL && code == OK)
    {
        pthread_mutex_unlock(&client->lock);
        print_log("Message incomplete\n");
        return;
    }

    client->request = message;
    client->code = code;

    empty_socket_fd(client);
}

static enum status_code compare_version(char *http_version)
{
    char *supported_version = HTTP_VERSION;
    // if the first 7 char are not the same
    if (strncmp(http_version, supported_version, 6))
        return BAD_REQUEST;

    if (strlen(http_version) != strlen(supported_version))
        return NOT_SUPPORTED;
    // if the eighth char is not a dot
    if (http_version[6] != supported_version[6])
        return BAD_REQUEST;
    if (http_version[5] != supported_version[5]
        || http_version[7] != supported_version[7])
        return NOT_SUPPORTED;

    return OK;
}

static enum status_code compare_host(char *incoming_host, char *hostname,
                                     char *servername,
                                     struct vhost_config *config)
{
    if (incoming_host == NULL)
        return BAD_REQUEST;

    if (!strcmp(incoming_host, hostname) || !strcmp(incoming_host, servername)
        || !strcmp(incoming_host, config->server_name)
        || !strcmp(incoming_host, config->ip))
        return OK;

    return BAD_REQUEST;
}

static enum status_code verify_message(struct http_message *message,
                                       char *hostname, char *servername,
                                       struct vhost_config *config)
{
    struct http_request *request = get_request(message);
    char *incoming_host = NULL;
    char *content_length = NULL;
    if (request == NULL || message == NULL)
        return BAD_REQUEST;

    enum status_code code = compare_version(request->http_version);

    if (code != OK)
        return code;

    if (request->method == METHOD_ERROR)
        return NOT_ALLOWED;

    if ((content_length = hash_map_get(message->headers, "Content-Length"))
        != NULL)
    {
        int length = atoi(content_length);
        if (length == 0 && content_length[0] != '0')
            length = -1;
        int len = strlen(message->body);
        if (len != length)
            return BAD_REQUEST;
    }

    if ((incoming_host = hash_map_get(message->headers, "host")) == NULL)
        return BAD_REQUEST;
    if (request->target[0] != '/')
        return BAD_REQUEST;

    code = compare_host(incoming_host, hostname, servername, config);

    if (code != OK)
        return code;

    return OK;
}

static void init_host_and_server_name(char *hostname, char *servername,
                                      struct vhost_config *config)
{
    if (hostname == NULL || servername == NULL)
        return;

    strcat(hostname, config->ip);
    strcat(hostname, ":");
    strcat(hostname, config->port);

    strcat(servername, config->server_name);
    strcat(servername, ":");
    strcat(servername, config->port);
}

static void init_date(char *date)
{
    time_t current_time = time(NULL);
    struct tm *tm = gmtime(&current_time);

    strftime(date, 4096, "%a, %e %b %Y %H:%M:%S GMT", tm);
}

static struct hash_map *init_headers(char *hostname, char *date)
{
    struct hash_map *headers = hash_map_init(STD_NB_HEADERS);

    hash_map_insert(headers, "Date", date, NULL);
    hash_map_insert(headers, "Connection", "close", NULL);
    hash_map_insert(headers, "Host", hostname, NULL);

    return headers;
}

static struct http_message *create_answer(enum status_code code,
                                          struct hash_map *headers,
                                          char *reasons[], char *body)
{
    return create_http_message(
        create_start_line(
            create_http_line(
                NULL, create_http_status(HTTP_VERSION, code, reasons[code])),
            STATUS),
        headers, body);
}

static int open_file(struct vhost_config *config, struct http_request *request,
                     enum status_code *code)
{
    char *filename = config->default_file;

    char *target = request->target;

    char path[4096] = { 0 };

    concat_path(path, config->root_dir, target);

    if (is_regular_file(path))
    {
        *code = OK;
        return open(path, O_RDONLY);
    }

    strcat(path, filename);

    if (!is_regular_file(path))
    {
        *code = NOT_FOUND;
        return -1;
    }

    *code = OK;
    return open(path, O_RDONLY);
}

static struct http_message *execute_request(struct http_request *request,
                                            struct hash_map *headers,
                                            struct vhost_config *config,
                                            char *reasons[])
{
    enum status_code code = OK;

    int fd = open_file(config, request, &code);

    int size;

    if (fd == -1 && code == OK)
        code = FORBIDDEN;
    else
        size = size_of_file(fd);

    struct http_message *answer = create_answer(code, headers, reasons, "");

    if (code == OK)
    {
        answer->file_to_serve = -1;

        if (request->method == GET)
        {
            answer->file_to_serve = fd;
        }
        else
            close(fd);
    }
    else
        size = 0;

    char *length = calloc(16, sizeof(char));
    sprintf(length, "%d", size);

    hash_map_insert(headers, "Content-Length", length, NULL);

    return answer;
}

static void write_answer(struct client *client, struct http_message *answer)
{
    struct http_status *status = get_status(answer);
    FILE *stream = client->stream;
    fprintf(stream, "%s %d %s\r\n", status->http_version, status->code,
            status->reason);

    hash_map_dump(stream, answer->headers);
    if (fprintf(stream, "\r\n") < 0)
    {
        if (answer->file_to_serve != -1)
        {
            close(answer->file_to_serve);
            return;
        }
    }

    if (answer->file_to_serve != -1)
    {
        char buf[4096] = { 0 };
        ssize_t read_data = 0;

        while ((read_data = read(answer->file_to_serve, buf, 4095)) > 0)
        {
            fputs(buf, stream);
            memset(buf, 0, 4096);
        }

        close(answer->file_to_serve);
    }
    else
    {
        fprintf(stream, "%s", answer->body);
    }
    fflush(stream);
}

static void prepare_client_after_write(struct client *client)
{
    client->request = NULL;

    client->made_writing = true;
    client->is_writing = false;
    client->is_finished = true;
}

void send_response(struct client *client, struct vhost_config *config)
{
    if (client == NULL || config == NULL)
        return;

    pthread_mutex_lock(&client->lock);
    if (client->request == NULL)
        process_message(client);

    if (client->request == NULL && client->code == OK)
        return;

    char hostname[4096] = { 0 };
    char servername[4096] = { 0 };
    char date[4096] = { 0 };
    char *length = NULL;
    init_host_and_server_name(hostname, servername, config);
    init_date(date);
    CREATE_REASON;

    struct hash_map *headers = init_headers(hostname, date);

    struct http_message *answer = NULL;
    struct http_message *message = client->request;
    enum status_code code = client->code;

    if (code == OK)
        code = verify_message(message, hostname, servername, config);

    if (code != OK)
        answer = create_answer(code, headers, reasons, "");
    else
    {
        answer =
            execute_request(get_request(message), headers, config, reasons);
    }
    if ((length = hash_map_get(headers, "Content-Length")) == NULL)
        hash_map_insert(headers, "Content-Length", "0", NULL);

    print_message(message);
    print_message(answer);

    print_request(get_request(message), get_status(answer), config, client->ip);
    print_response(get_request(message), get_status(answer), config,
                   client->ip);

    write_answer(client, answer);

    if (length)
        free(length);
    prepare_client_after_write(client);

    free_http_message(answer);
    free_http_message(message);

    pthread_mutex_unlock(&client->lock);
}
