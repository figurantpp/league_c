
#ifndef ZALLOC_H
#define XALLOC_H



#include <stdlib.h>

void *zmalloc(size_t size);

void *zrealloc(void *ptr, size_t);

void *zcalloc(size_t nmemb, size_t size);

void z_alloc_set_abort_function(void (*abort_function)());

#endif