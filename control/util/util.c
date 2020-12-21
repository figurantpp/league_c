//
// Created by figurantpp on 21/11/2020.
//

#include <zalloc/zalloc.h>
#include "util.h"



char *safe_str_dup(char *str)
{
    return str ? strdup(str) : NULL;
}

void *mem_dup(void *memory, size_t length)
{
    if (memory == NULL || length == 0)
    {
        return NULL;
    }

    void *buffer = zmalloc(length);
    memcpy(buffer, memory, length);
    return buffer;
}


const char* left_trim_string(const char *str)
{
    while (*str == ' ')
    {
        str++;
    }

    return str;
}

// NOTE: Modifies str
void right_trim_string(char *str)
{
    char *position = str + strlen(str) - 1;


    while (*position == ' ')
    {
        position--;
    }

    *(position + 1) = '\0';
}


char *full_trim_string(char *str)
{
    right_trim_string(str);
    return (char*) left_trim_string(str);
}
