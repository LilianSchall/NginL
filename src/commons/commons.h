#ifndef COMMONS_H
#define COMMONS_H

#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void free_array(void *data, size_t nb_elt, size_t size);

void free_array_content(void *data, size_t nb_elt, size_t size);
char *strtrim(char *str);

int find(int *array, size_t size, int elt);

void concat_path(char *dst, char *path1, char *path2);
bool is_regular_file(const char *path);
int size_of_file(int filefd);

int my_strcasecmp(const char *s1, const char *s2);
void my_strlowcase(char *str);

void set_non_blocking(int fd);
#endif /* !COMMONS_H */
