#include "client.h"

struct client *create_client(int fd, char *ip)
{
    if (fd == -1 || ip == NULL)
        return NULL;

    struct client *client = malloc(sizeof(struct client));

    if (client == NULL)
        return NULL;

    client->fd = fd;
    client->stream = NULL;
    client->ip = ip;
    pthread_mutex_init(&client->lock, NULL);
    pthread_mutex_init(&client->reading_ready, NULL);
    client->request = NULL;
    client->answer = NULL;

    client->code = OK;

    client->is_finished = false;
    client->made_writing = false;
    client->is_writing = false;
    return client;
}

void launch_client(struct client *client)
{
    if (client == NULL)
        return;

    client->stream = fdopen(dup(client->fd), "r+");
}

void stop_client(struct client *client)
{
    if (client == NULL)
        return;

    fflush(client->stream);
    if (client->stream)
        fclose(client->stream);

    if (client->fd != -1)
        close(client->fd);
}

void free_client(void *data)
{
    if (data == NULL)
        return;

    struct client *client = data;

    if (client->request != NULL)
        free_http_message(client->request);
    if (client->answer != NULL)
        free_http_message(client->answer);
    if (client->ip != NULL)
        free(client->ip);

    pthread_mutex_destroy(&client->lock);

    free(client);
}
