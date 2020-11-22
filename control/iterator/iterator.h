/*
 * Created by figurantpp on 06/11/2020.
 */

#ifndef IT_ITERATOR_H
#define IT_ITERATOR_H

struct Iterator;

typedef int IteratorCallback(void *out, struct Iterator *iterator);


typedef void IteratorDeletionCallback(struct Iterator *iterator);

struct Iterator
{
    IteratorCallback *next;
    IteratorDeletionCallback *delete;
    void *state;
    unsigned char auto_delete;
};

/* Yeah i know, it looks ugly, but hey, it works good =] */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

#define ITERATE(x, iterator) __ITERATOR_INTERNAL_ITERATE(x, iterator)

#define __ITERATOR_INTERNAL_ITERATE(x, iterator) \
for                                             \
(                                           \
    struct Iterator* __ITERATOR_INTERNAL_IDENTIFIER = iterator; \
    __ITERATOR_INTERNAL_IDENTIFIER->next(&x, __ITERATOR_INTERNAL_IDENTIFIER); \
)


#define __ITERATOR_INTERNAL_IDENTIFIER \
    __ITERATOR_INTERNAL_IDENTIFIER_CAT(internal_iterator_macro_identifier_numbered, __LINE__)

#define __ITERATOR_INTERNAL_IDENTIFIER_CAT(x, y) __ITERATOR_INTERNAL_IDENTIFIER_INNER_CAT(x,y)
#define __ITERATOR_INTERNAL_IDENTIFIER_INNER_CAT(x, y) x ## y

#endif

#define FOR_EVERY(x, iterator)  \
    while (iterator->next(&x, iterator))


#endif /* IT_ITERATOR_H */
