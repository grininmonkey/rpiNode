#include <stdio.h>
#include <string.h>
#include "assignValues.h"

void mpu6050_assign_values(SharedData *node, int index, const char *idAppend,  float f_value, const char *timestamp) {
    const char *id = "MPU6050";
    const char *type = "i2c";
    const char *description = "MPU6050 device accel/gyro";
    
    pthread_mutex_lock(&node->lock);
    snprintf(node->internal.MPU6050.values[index].info.id, RPI_MAX_META_ID, "%s-%s", id, idAppend);
    SAFE_STRCPY(node->internal.MPU6050.values[index].info.type, type);
    SAFE_STRCPY(node->internal.MPU6050.values[index].info.description, description);
    SAFE_STRCPY(node->internal.MPU6050.values[index].info.timestamp, timestamp);
    snprintf(node->internal.MPU6050.values[index].value, RPI_MAX_VALUE_LENGTH, "%f", f_value);
    pthread_mutex_unlock(&node->lock);
}