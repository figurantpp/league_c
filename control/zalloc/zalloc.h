
#ifndef ZALLOC_H
#define ZALLOC_H



#include <stdlib.h>

__attribute__((alloc_size(1)))
__attribute__((malloc))
__attribute__((returns_nonnull))
__attribute__((warn_unused_result))
void *zmalloc(size_t size);

__attribute__((alloc_size(2)))
__attribute__((returns_nonnull))
__attribute__((warn_unused_result))
void *zrealloc(void *ptr, size_t);

__attribute__((alloc_size(1,2)))
__attribute__((malloc))
__attribute__((returns_nonnull))
__attribute__((warn_unused_result))
void *zcalloc(size_t nmemb, size_t size);

void z_alloc_set_abort_function(void (*abort_function)());

#endif