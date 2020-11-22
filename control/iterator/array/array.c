/*
 * Created by figurantpp on 06/11/2020.
 */

#include "array.h"

#include <stdlib.h>
#include <iterator/iterator.h>
#include <string.h>

struct ArrayIteratorState
{
    const void *current;
    size_t remaining_count;
    size_t element_size;
};

static void array_iterator_delete(struct Iterator *iterator)
{
    if (iterator)
    {
        free(iterator->state);
        free(iterator);
    }
}

static int array_iterator_next(void *out, struct Iterator *iterator)
{
    struct ArrayIteratorState *state = iterator->state;

    if (state->remaining_count == 0)
    {
        if (iterator->auto_delete)
        {
            iterator->delete(iterator);
        }

        return 0;
    }

    memcpy(out, state->current, state->element_size);

    state->current = (char*) state->current + state->element_size;

    state->remaining_count--;

    return 1;
}

struct Iterator *array_iterator(const void *array, size_t count, size_t size)
{
    struct Iterator *iterator = malloc(sizeof(struct Iterator));
    struct ArrayIteratorState *state;

    if (!iterator)
    {
        return NULL;
    }

    state = malloc(sizeof(struct ArrayIteratorState));

    if (!state)
    {
        free(iterator);
        return NULL;
    }

    state->current = array;
    state->remaining_count = count;
    state->element_size = size;

    iterator->state = state;

    iterator->next = array_iterator_next;
    iterator->delete = array_iterator_delete;
    iterator->auto_delete = 1;

    return iterator;
}

struct Iterator *string_iterator(const char *string)
{
    return TO_ITERATOR(string, strlen(string));
}
