#include "communication.h"

static int choose_available_socket(struct addrinfo *res)
{
    int sockfd = -1;
    for (; res != NULL; res = res->ai_next)
    {
        if ((sockfd =
                 socket(res->ai_family, res->ai_socktype, res->ai_protocol))
            == -1)
        {
            print_log("socket");
            continue;
        }

        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
            == -1)
        {
            print_log("setsockopt");
            close(sockfd);
            sockfd = -1;
            continue;
        }

        if (bind(sockfd, res->ai_addr, res->ai_addrlen) != -1)
            break;

        close(sockfd);
        sockfd = -1;
    }
    return sockfd;
}
struct communication *create_communication(char *ip, char *port)
{
    if (ip == NULL || port == NULL)
        return NULL;

    struct communication *com = malloc(sizeof(struct communication));

    if (com == NULL)
        return NULL;

    int err;
    struct addrinfo *res;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((err = getaddrinfo(ip, port, &hints, &res)) != 0)
    {
        print_log(gai_strerror(err));
        freeaddrinfo(res);
        free(com);
        return NULL;
    }

    com->res = res;
    int sockfd = choose_available_socket(res);

    struct client **clients = calloc(CLIENT_CAPACITY, sizeof(struct client *));

    if (clients == NULL || (listen(sockfd, 10) == -1))
    {
        close(sockfd);
        free(com);
        freeaddrinfo(res);

        if (clients != NULL)
            free(clients);

        return NULL;
    }

    com->fd = sockfd;
    com->clients = clients;
    com->client_capacity = CLIENT_CAPACITY;
    com->client_size = 0;

    return com;
}

void add_client_communication(struct communication *com, int client_fd,
                              struct sockaddr client)
{
    char *ip = calloc(INET_ADDRSTRLEN, sizeof(char));
    void *tmp = &client;
    struct sockaddr_in *ip_addr = tmp;
    inet_ntop(AF_INET, &ip_addr->sin_addr, ip, INET_ADDRSTRLEN);

    struct client *new_client = create_client(client_fd, ip);

    if (new_client == NULL)
        return;

    bool write = false;

    for (size_t i = 0; i < com->client_size; i++)
    {
        if (com->clients[i] == NULL)
        {
            write = true;
            com->clients[i] = new_client;
            break;
        }
    }

    if (write)
        return;

    if (com->client_size + 1 >= com->client_capacity)
    {
        com->clients = realloc(
            com->clients, com->client_capacity * 2 * sizeof(struct client *));

        com->client_capacity *= 2;
    }
    com->clients[com->client_size++] = new_client;
}

struct client *find_client(struct communication *com, int fd)
{
    if (com == NULL)
        return NULL;

    for (size_t i = 0; i < com->client_size; i++)
    {
        if (com->clients[i] == NULL)
            continue;

        if (com->clients[i]->fd == fd)
            return com->clients[i];
    }

    return NULL;
}

void stop_communication(struct communication *com)
{
    if (com == NULL)
        return;

    for (size_t i = 0; i < com->client_size; i++)
    {
        if (com->clients[i] == NULL)
            continue;

        stop_client(com->clients[i]);
        free_client(com->clients[i]);
        com->clients[i] = NULL;
    }

    close(com->fd);
}

void free_communication(struct communication *com)
{
    if (com == NULL)
        return;

    close(com->fd);
    freeaddrinfo(com->res);
    free(com->clients);
    free(com);
}
