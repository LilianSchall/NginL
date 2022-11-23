#include <stdlib.h>
#include <string.h>

#include "hash_map.h"

void hash_map_dump(FILE *stream, struct hash_map *hash_map)
{
    if (hash_map == NULL)
        return;

    for (size_t i = 0; i < hash_map->size; i++)
    {
        struct pair_list *list = hash_map->data[i];
        if (list == NULL)
            continue;

        while (list != NULL)
        {
            struct pair_list *next = list->next;

            fprintf(stream, "%s: %s", list->key, list->value);

            fprintf(stream, "\r\n");

            list = next;
        }
    }
}
