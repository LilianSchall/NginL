#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "commons/commons.h"

struct pair_list
{
    char *key;
    char *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    size_t size;
};

size_t hash(const char *str);

struct hash_map *hash_map_init(size_t size);
void hash_map_free(struct hash_map *hash_map, bool free_kvp);

bool hash_map_insert(struct hash_map *hash_map, char *key, char *value,
                     bool *updated);
void hash_map_dump(FILE *stream, struct hash_map *hash_map);
char *hash_map_get(const struct hash_map *hash_map, char *key);
bool hash_map_remove(struct hash_map *hash_map, char *key);

#endif /* !HASH_MAP_H */
