/*
 * Created by figurantpp on 06/11/2020.
 */

#ifndef IT_RANGE_H
#define IT_RANGE_H

#include <stddef.h>

#include <iterator/iterator.h>

struct Iterator *range(size_t start, size_t end, size_t increment);

#endif /* IT_RANGE_H */
