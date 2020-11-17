/*
 * Created by figurantpp on 16/11/2020.
 */

#ifndef FULLMACRO_DECONSTRUCT_H
#define FULLMACRO_DECONSTRUCT_H

#include <stdlib.h>

#define __DECONSTRUCT_POINTER_CLEANUP(name) __POINTER_DESCONSTRUCTOR_OF__##name##__

#define SET_POINTER_DECONSTRUCTOR(type, function) \
void __DECONSTRUCT_POINTER_CLEANUP(type) (type** __element) { function(*__element); }

#define SET_POINTER_DECONSTRUCTOR_STATIC(type, function) \
static void __DECONSTRUCT_POINTER_CLEANUP(type) (type** __element) { function(*__element); }

#define ON_DELETE(callback) __attribute__((cleanup(callback)))

static void deconstruct_internal_free(void *double_pointer)
{
    void **real = double_pointer;
    free(*real);
}


#define UNIQUE_POINTER(type) \
    void __POINTER_DESCONSTRUCTOR_OF__##type##__(type**); \
    __attribute__((cleanup(__DECONSTRUCT_POINTER_CLEANUP(type)))) type*

#define AUTO_FREE ON_DELETE(deconstruct_internal_free)


#endif /* FULLMACRO_DECONSTRUCT_H */
