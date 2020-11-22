/*
 * Created by figurantpp on 06/11/2020.
 */

#include "range.h"
#include <stdlib.h>

static void range_delete(struct Iterator *range)
{
    if (range)
    {
        free(range->state);
        free(range);
    }
}

/* private */
struct RangeState
{
    size_t current;
    size_t end;
    size_t increment;
};

static int range_next(void *out, struct Iterator* iterator)
{
    size_t *output = out;
    struct RangeState* state = iterator->state;

    if (state->current >= state->end)
    {
        if (iterator->auto_delete)
        {
            iterator->delete(iterator);
        }

        return 0;
    }

    *output = state->current;

    state->current += state->increment;

    return 1;
}

struct Iterator *range(size_t start, size_t end, size_t increment)
{
    struct Iterator *iterator = malloc(sizeof(struct Iterator));
    struct RangeState *state;

    if (!iterator)
    {
        return NULL;
    }

    state = malloc(sizeof(struct RangeState));

    if (!state)
    {
        free(iterator);
        return NULL;
    }

    state->current = start;
    state->end = end;
    state->increment = increment;

    iterator->next = range_next;
    iterator->state = state;
    iterator->delete = range_delete;
    iterator->auto_delete = 1;

    return iterator;
}
