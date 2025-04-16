#ifndef UTILS_SET_SHARED_VALUE_H
#define UTILS_SET_SHARED_VALUE_H

#define SINGLE_INSTANCE -1
#include "../structs/rpiNode.h"

void set_shared_multivalue(
    MultiValue *obj,
    int obj_index,
    int value_index, 
    char *id,
    char *type,
    char *description,
    float f_value,
    const char *c_value
);

#endif