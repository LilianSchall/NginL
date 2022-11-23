#include "commons.h"

void free_array(void *data, size_t nb_elt, size_t size)
{
    if (data == NULL)
        return;

    free_array_content(data, nb_elt, size);

    free(data);
}

void free_array_content(void *data, size_t nb_elt, size_t size)
{
    if (data == NULL)
        return;
    char **converted = data;

    for (size_t i = 0; i < nb_elt * size; i += size)
    {
        char *ptr = *(converted + i);
        free(ptr);
    }
}

int find(int *array, size_t size, int elt)
{
    if (array == NULL)
        return -1;

    for (size_t i = 0; i < size; i++)
    {
        if (array[i] == elt)
            return i;
    }
    return -1;
}
char *strtrim(char *str)
{
    size_t i = 0;

    while (isspace(str[i]))
        i++;

    return str + i;
}

void concat_path(char *dst, char *path1, char *path2)
{
    if (dst == NULL || path1 == NULL || path2 == NULL)
        return;

    size_t len1 = strlen(path1);

    bool got_slash1 = path1[len1 - 1] == '/';
    bool got_slash2 = path2[0] == '/';

    strcat(dst, path1);

    if (!(got_slash1 || got_slash2))
        strcat(dst, "/");
    strcat(dst, path2);
}

bool is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);

    return S_ISREG(path_stat.st_mode);
}

int size_of_file(int filefd)
{
    off_t begin = lseek(filefd, 0, SEEK_CUR);
    off_t end = lseek(filefd, 0, SEEK_END);

    lseek(filefd, begin, SEEK_SET);

    return end;
}

static char char_low(char c)
{
    if ('A' <= c && c <= 'Z')
        return c + 32;
    return c;
}

static int char_low_cmp(char c1, char c2)
{
    return char_low(c1) > char_low(c2);
}

void my_strlowcase(char *str)
{
    for (int i = 0; str[i] != 0; i += 1)
        str[i] = char_low(str[i]);
}

int my_strcasecmp(const char *s1, const char *s2)
{
    size_t i = 0;
    while (s1[i] && s2[i] != 0)
    {
        if (char_low_cmp(s1[i], s2[i]))
            return 1;
        if (char_low_cmp(s2[i], s1[i]))
            return -1;

        i++;
    }

    if (s1[i] == 0 && s2[i] == 0)
        return 0;

    return s1[i] != 0 ? 1 : -1;
}

void set_non_blocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}
