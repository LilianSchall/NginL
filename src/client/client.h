#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>

#include "http/http_message/http_message.h"

struct client
{
    int fd;
    FILE *stream;
    pthread_mutex_t lock;
    pthread_mutex_t reading_ready;
    char *ip;

    struct http_message *request;
    struct http_message *answer;

    enum status_code code;

    bool is_finished;
    bool made_writing;
    bool is_writing;
};

struct client *create_client(int fd, char *ip);

void launch_client(struct client *client);

void stop_client(struct client *client);

void free_client(void *data);

#endif /* !CLIENT_H */
