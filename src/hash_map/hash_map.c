#include "hash_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *hm = malloc(sizeof(struct hash_map));

    if (hm == NULL)
        return NULL;

    struct pair_list **data = calloc(size, sizeof(struct hash_map *));

    if (data == NULL)
    {
        free(hm);
        return NULL;
    }

    hm->data = data;
    hm->size = size;

    return hm;
}

void hash_map_free(struct hash_map *hash_map, bool free_kvp)
{
    if (hash_map == NULL)
        return;

    for (size_t i = 0; i < hash_map->size; i++)
    {
        struct pair_list *list = hash_map->data[i];
        while (list != NULL)
        {
            struct pair_list *next = list->next;
            if (free_kvp)
            {
                free(list->key);
                free(list->value);
            }
            free(list);
            list = next;
        }
    }
    free(hash_map->data);
    free(hash_map);
}

struct pair_list *create_list(char *key, char *value, struct pair_list *next)
{
    struct pair_list *list = malloc(sizeof(struct pair_list));

    if (list == NULL)
        return NULL;

    list->next = next;
    list->key = key;
    list->value = value;

    return list;
}

struct pair_list *find_key(struct pair_list *list, char *key)
{
    if (list == NULL)
        return NULL;

    while (list != NULL)
    {
        if (my_strcasecmp(list->key, key) == 0)
            return list;

        list = list->next;
    }

    return NULL;
}

bool hash_map_insert(struct hash_map *hash_map, char *key, char *value,
                     bool *updated)
{
    if (hash_map == NULL || key == NULL || hash_map->size == 0)
        return false;

    size_t hashcode = hash(key) % hash_map->size;

    struct pair_list *list = find_key(hash_map->data[hashcode], key);

    if (list != NULL)
    {
        // new value for key
        return false;
    }
    else
    {
        // we found nothing
        struct pair_list *list =
            create_list(key, value, hash_map->data[hashcode]);

        if (list == NULL)
            return NULL;

        hash_map->data[hashcode] = list;
        if (updated != NULL)
            *updated = false;
    }

    return true;
}

char *hash_map_get(const struct hash_map *hash_map, char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
        return NULL;

    size_t hashcode = hash(key) % hash_map->size;

    struct pair_list *list = find_key(hash_map->data[hashcode], key);

    if (list == NULL)
        return NULL;

    return list->value;
}

bool hash_map_remove(struct hash_map *hash_map, char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
        return false;

    size_t hashcode = hash(key) % hash_map->size;

    struct pair_list *list = hash_map->data[hashcode];
    struct pair_list *parent = NULL;

    while (list != NULL && strcmp(list->key, key) != 0)
    {
        parent = list;
        list = list->next;
    }

    if (list == NULL)
        return false;

    // we found an element

    if (parent == NULL)
        hash_map->data[hashcode] = list->next;
    else
    {
        parent->next = list->next;
    }

    free(list);
    return true;
}
