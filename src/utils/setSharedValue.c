#include <string.h>
#include <stdio.h>
#include "getTimeStamp.h"
#include "setSharedValue.h"

void set_shared_multivalue(
    MultiValue *obj,
    int obj_index,
    int value_index, 
    char *id,
    char *type,
    char *description,
    float f_value,
    const char *c_value
) {
    if (!obj)
        return;

    MultiValue *target = obj;

    //-----------------------------------------------------------------
    // Handle arrays vs single instance
    //-----------------------------------------------------------------
    /*
    if (obj_index != SINGLE_INSTANCE) {
        if (obj_index >= 0 && obj_index < RPI_MAX_INTERNAL_OBJECTS) {
            target = &obj[obj_index];
        } else {
            fprintf(stderr, "Warning: obj_index %d out of bounds (max: %d)\n", obj_index, RPI_MAX_INTERNAL_OBJECTS);
            return;
        }
    } 
    */
    //-----------------------------------------------------------------
    // 'target' is safe to use
    //-----------------------------------------------------------------
    if (value_index >= 0 && value_index < RPI_MAX_VALUES_PER_OBJECT && id) {
        SingleValue *val = &target->values[value_index];
        pthread_mutex_lock(&rpiNode.lock);
        SAFE_STRCPY(val->info.id, id);
        SAFE_STRCPY(val->info.type, (type) ? type : "unknown");
        if (description) SAFE_STRCPY(val->info.description, description);
        SAFE_STRCPY(val->info.timestamp, get_timestamp());
        if (c_value != NULL) {
            SAFE_STRCPY(val->value, c_value);
        } else {
            snprintf(val->value, RPI_MAX_VALUE_LENGTH, "%f", f_value);
        }
        pthread_mutex_unlock(&rpiNode.lock);        

    } else {
        fprintf(stderr, "Warning: value_index %d out of bounds\n", value_index);
    }


}