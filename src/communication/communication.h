#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define CLIENT_CAPACITY 16

#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "client/client.h"
#include "linked_list/linked_list.h"
#include "log/log.h"

struct communication
{
    int fd;
    struct addrinfo *res;
    struct client **clients;
    size_t client_capacity;
    size_t client_size;
};

#include "job/create_specific_job.h"

struct communication *create_communication(char *ip, char *port);

void add_client_communication(struct communication *com, int client_fd,
                              struct sockaddr client);

struct client *find_client(struct communication *com, int fd);

void stop_communication(struct communication *com);

void free_communication(struct communication *com);

#endif /* !COMMUNICATION_H */
