#include <stdio.h>
#include <string.h>
#include "../../utils/setSharedValue.h"

void mpu6050_assign_values(int index, const char *idAppend,  float f_value) {
    char id[RPI_MAX_META_ID];
  
    snprintf(id, RPI_MAX_META_ID, "MPU6050-%s", idAppend);

    set_shared_multivalue(
        &rpiNode.internal.MPU6050,
        SINGLE_INSTANCE,
        index, 
        id,
        "i2c",
        "MPU6050 device accel/gyro",
        f_value,
        NULL
    );
}