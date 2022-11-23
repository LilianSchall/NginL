#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <pthread.h>
#include <stdlib.h>

struct linked_node
{
    void *data;
    struct linked_node *next;
};

struct linked_list
{
    struct linked_node *head;
    pthread_mutex_t lock;
};

struct linked_list *list_create(void);

void *list_head(struct linked_list *list);
size_t list_size(struct linked_list *list);

struct linked_list *list_append(struct linked_list *list, void *data);
struct linked_list *list_pop(struct linked_list *list);
struct linked_list *list_pop_non_safe(struct linked_list *list);

void list_deep_free(struct linked_list *list, void (*free_fct)(void *));
void list_free(struct linked_list *list);

#endif /* !LINKED_LIST_H */
