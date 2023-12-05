#include "headers.h"
#include "path_conversion.h"

char *no_prefix(char *path)
{
    // INPUT = dir => OUTPUT = dir
    if (path[0] != '.')
    {
        return path;
    }
    // INPUT = dir => OUTPUT = dir
    int length = strlen(path);
    char *ret = (char *)calloc(length + 5, sizeof(char));

    int i = 0;
    while (path[i + 2] != '\0')
    {
        ret[i] = path[i + 2];
        i++;
    }
    ret[i] = '\0';
    return ret;
}

char *prefix(char *path)
{
    // INPUT = ./dir => OUPUT = ./dir
    if (path[0] == '.')
    {
        return path;
    }
    // INPUT = dir => OUTPUT = ./dir
    int length = strlen(path);
    char *ret = (char *)calloc(length + 5, sizeof(char));

    ret[0] = '.';
    ret[1] = '/';
    int i = 0;
    while (path[i] != '\0')
    {
        ret[i + 2] = path[i];
        i++;
    }
    ret[i + 2] = '\0';
    return ret;
}