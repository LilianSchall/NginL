#include <stddef.h>

#include "hash_map.h"

size_t hash(const char *key)
{
    char *array = strdup(key);
    my_strlowcase(array);

    size_t i = 0;
    size_t hash = 0;

    for (i = 0; array[i] != '\0'; ++i)
        hash += array[i];
    hash += i;

    free(array);

    return hash;
}
