
#include <stdlib.h>
#include <fullmacro/deconstruct.h>
#include <memory.h>
#include "zalloc.h"

static void (*abort_function)() = abort;


void *zmalloc(size_t size)
{
    void *result = malloc(size);

    if (size && !result)
    {
        abort_function();
    }

    return result;
}

void *zrealloc(void *ptr, size_t size)
{
    void *result = realloc(ptr, size);

    if (size && !result)
    {
        abort_function();
    }

    return result;
}

void *zcalloc(size_t nmemb, size_t size)
{
    void *result = calloc(nmemb, size);

    if (size && !result)
    {
        abort_function();
    }

    return result;
}

void z_alloc_set_abort_function(void (*function)())
{
    abort_function = function;
}

char *zstrdup(const char *str)
{
    if (str == NULL)
    {
        return NULL;
    }

    char *result = strdup(str);

    if (!result)
    {
        abort_function();
    }

    return result;
}

// UNIQUE_POINTER(FILE) definitions

#include <stdio.h>

static void file_free(FILE *file)
{
    if (file)
    {
        fclose(file);
    }
}

SET_POINTER_DECONSTRUCTOR(FILE, file_free);
