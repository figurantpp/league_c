/*
 * Created by figurantpp on 06/11/2020.
 */

#ifndef IT_ARRAY_H
#define IT_ARRAY_H

#include <stddef.h>

struct Iterator *array_iterator(const void *array, size_t count, size_t size);

#define TO_ITERATOR(array, count) (array_iterator(array, count, sizeof(*array)))

struct Iterator *string_iterator(const char *string);

#endif /* IT_ARRAY_H */
