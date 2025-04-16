#include <string.h>
#include <stdio.h>
#include "../structs/rpiNode.h"

int copy_singlevalue_to_tags(SingleValue *src, SingleValue *dest_array, int dest_index) {
    if (!src || !dest_array) return dest_index;
    if (dest_index >= RPI_MAX_TAGS) return dest_index;

    if (src->info.id[0] != '\0') {
        dest_array[dest_index].info = src->info;
        SAFE_STRCPY(dest_array[dest_index].value, src->value);
        return dest_index + 1;
    }

    return dest_index;
}

int update_tags() {

    int index = 0;
    pthread_mutex_lock(&rpiNode.lock);

    //memset(rpiNode.tags, 0, sizeof(rpiNode.tags)); // zero the DATA 

    // -------------------------------------------------------------------------------
    // DS18B20 values
    // -------------------------------------------------------------------------------
    for (int d_Index = 0; d_Index < RPI_MAX_INTERNAL_OBJECTS; d_Index++) {
        for (int z = 0; z < rpiNode.internal.DS18B20[d_Index].values_count; z++) {
            SingleValue *v = &rpiNode.internal.DS18B20[d_Index].values[z];
            index = copy_singlevalue_to_tags(v, rpiNode.tags, index);
        }
    }    
    // -------------------------------------------------------------------------------
    // MPU6050 values
    // -------------------------------------------------------------------------------
    for (int i = 0; i < rpiNode.internal.MPU6050.values_count; i++) {
        SingleValue *v = &rpiNode.internal.MPU6050.values[i];
        index = copy_singlevalue_to_tags(v, rpiNode.tags, index);
    } 


    rpiNode.tags_count = index;

    pthread_mutex_unlock(&rpiNode.lock);

    return 1;
}
