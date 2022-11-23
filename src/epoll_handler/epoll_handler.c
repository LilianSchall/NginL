#include "epoll_handler.h"

struct epoll_handler *create_epoll_handler(struct communication *com)
{
    if (com == NULL)
        return NULL;

    struct epoll_handler *ep = malloc(sizeof(struct epoll_handler));

    if (ep == NULL)
        return NULL;

    int epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        free(ep);
        return NULL;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    set_non_blocking(com->fd);
    ev.data.fd = com->fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, com->fd, &ev) == -1)
    {
        free(ep);
        return NULL;
    }

    ep->fd = epollfd;
    pthread_mutex_init(&ep->lock, NULL);

    return ep;
}

static bool add_new_client_to_server(struct epoll_handler *ep,
                                     struct communication *com)
{
    struct sockaddr client;
    socklen_t addr_size;
    int client_fd;
    struct epoll_event ev;

    while (1)
    {
        if ((client_fd = accept(com->fd, &client, &addr_size)) == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return true;

            print_log("add_new_client_to_server: client failed to accept\n");
            return true;
        }

        add_client_communication(com, client_fd, client);
        set_non_blocking(client_fd);

        ev.data.fd = client_fd;
        ev.events = EPOLLIN | EPOLLET | EPOLLOUT;

        if (epoll_ctl(ep->fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
        {
            print_log("add_new_client_to_server: failed to add client to "
                      "epoll_ctl\n");
            return false;
        }
    }
    return true;
}

static struct job *handle_client_event(struct epoll_handler *ep,
                                       struct communication *com,
                                       struct vhost_config *config, int n)
{
    struct epoll_event ev = ep->events[n];
    struct client *client = find_client(com, ev.data.fd);
    if (client == NULL)
        return NULL;

    struct job *job = NULL;

    char buf[64];
    print_log("Reading events for fd = %d... ", client->fd);
    if ((ev.events & EPOLLIN) && client->made_writing)
    {
        // ready for reading
        print_log("Client is ready for reading\n");
        if (client->made_writing)
        {
            int nbytes =
                recv(client->fd, buf, sizeof(buf), MSG_PEEK | MSG_DONTWAIT);
            print_log("nb_bytes min: %d\n", nbytes);
            if (nbytes == 0)
            {
                client->is_finished = true;
                pthread_mutex_unlock(&client->lock);
                return NULL;
            }
            else
            {
                print_log("content: %s\n", buf);
            }
        }
        job = NULL; // create_send_response_job(client, config);
    }
    else if (ev.events & EPOLLOUT && !client->is_writing)
    {
        print_log("Client is ready for writing\n");

        if (client->stream == NULL)
        {
            // we haven't read for this client yet
            launch_client(client);
        }
        client->is_writing = true;
        job = create_send_response_job(client, config);
    }

    return job;
}

void close_finished_clients(struct epoll_handler *ep, struct communication *com)
{
    if (com == NULL || ep == NULL)
        return;

    pthread_mutex_lock(&ep->lock);

    for (size_t i = 0; i < com->client_size; i++)
    {
        if (com->clients[i] == NULL
            || pthread_mutex_trylock(&com->clients[i]->lock) == EBUSY)
            continue;

        if (!com->clients[i]->is_finished)
        {
            pthread_mutex_unlock(&com->clients[i]->lock);
            continue;
        }
        int fd = com->clients[i]->fd;
        if (epoll_ctl(ep->fd, EPOLL_CTL_DEL, com->clients[i]->fd, NULL) == -1)
        {
            free_client(com->clients[i]);
            continue;
        }

        stop_client(com->clients[i]);
        free_client(com->clients[i]);

        com->clients[i] = NULL;
        print_log("Closed client %d\n", fd);
    }

    pthread_mutex_unlock(&ep->lock);
}

bool use_epoll_handler(struct epoll_handler *ep, struct communication *com,
                       struct vhost_config *config,
                       struct linked_list *job_queue)
{
    if (ep == NULL || com == NULL)
    {
        return false;
    }
    pthread_mutex_lock(&ep->lock);
    struct job *job = NULL;
    int nfds = epoll_wait(ep->fd, ep->events, MAX_EVENTS, 200);

    for (int n = 0; n < nfds; n++)
    {
        if (ep->events[n].data.fd == com->fd)
        {
            print_log("New interaction with server\n");
            if (!add_new_client_to_server(ep, com))
            {
                continue;
            }
        }
        else
        {
            print_log("New interaction with client\n");
            if ((job = handle_client_event(ep, com, config, n)) != NULL)
            {
                job_queue = list_append(job_queue, job);
            }
            else
            {
                print_log("Job is null\n");
            }
        }
    }

    pthread_mutex_unlock(&ep->lock);
    return true;
}

void free_epoll_handler(struct epoll_handler *ep, struct communication *com)
{
    if (ep == NULL || com == NULL)
        return;

    pthread_mutex_destroy(&ep->lock);

    for (size_t i = 0; i < com->client_size; i++)
    {
        if (com->clients[i] == NULL)
            continue;

        epoll_ctl(ep->fd, EPOLL_CTL_DEL, com->clients[i]->fd, NULL);
    }

    free(ep);
}
