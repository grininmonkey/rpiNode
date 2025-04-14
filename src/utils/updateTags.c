//#include <stdio.h>
#include <string.h>
#include "../structs/rpiNode.h"

/*
static void add_tag (int *index, const char* id, const char* description, 
    const char* type, const char* timestamp, const char* value) 
{
    SAFE_STRCPY(rpiNode.tags[*index].info.id, id);
    SAFE_STRCPY(rpiNode.tags[*index].info.type, type);
    SAFE_STRCPY(rpiNode.tags[*index].info.description, description);
    SAFE_STRCPY(rpiNode.tags[*index].info.timestamp, timestamp);
    SAFE_STRCPY(rpiNode.tags[*index].value, value);
    (*index)++;    
}
*/

int update_tags() {

    int index = 0;
    pthread_mutex_lock(&rpiNode.lock);

    memset(rpiNode.tags, 0, sizeof(rpiNode.tags)); // zero the DATA 

    // -------------------------------------------------------------------------------
    // DS18B20 values
    // -------------------------------------------------------------------------------
    for (int i = 0; i < RPI_MAX_DS18B20S; i++) {
        for (int z = 0; z < rpiNode.internal.DS18B20[i].values_count; z++) {
            if (rpiNode.internal.DS18B20[i].values[z].info.id[0] != '\0') {
                rpiNode.tags[index].info = rpiNode.internal.DS18B20[i].values[z].info;
                SAFE_STRCPY(rpiNode.tags[index].value, rpiNode.internal.DS18B20[i].values[z].value);
                index += 1;
            }
        }
    }    
    // -------------------------------------------------------------------------------
    // MPU6050 values
    // -------------------------------------------------------------------------------
    for (int i = 0; i < rpiNode.internal.MPU6050.values_count; i++) {
        if (rpiNode.internal.MPU6050.values[i].info.id[0] != '\0') {
            rpiNode.tags[index].info = rpiNode.internal.MPU6050.values[i].info;
            SAFE_STRCPY(rpiNode.tags[index].value, rpiNode.internal.MPU6050.values[i].value);
            index += 1;
        }
    } 


    rpiNode.tags_count = index;

    pthread_mutex_unlock(&rpiNode.lock);

    return 1;
}
