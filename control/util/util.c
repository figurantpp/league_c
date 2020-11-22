//
// Created by figurantpp on 21/11/2020.
//

#include "util.h"



char *safe_dup(char *str)
{
    return str ? strdup(str) : NULL;
}
