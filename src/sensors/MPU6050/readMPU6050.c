#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <linux/i2c-dev.h>
#include "smoothingMovingAverage.h"
#include "calculateAngles.h"
#include "../../utils/getTimeStamp.h"
#include "assignValues.h"
#include "../../utils/signalHandler.h"
#include "../../utils/getTimeStamp.h"
#include "../../utils/sleepMs.h"
#include "../../utils/nameValue.h"
#include "../../structs/rpiNode.h"

#define MPU_ADDR 0x68

int read_word_2c(char high, char low) {
    int val = (high << 8) | low;
    if (val >= 0x8000)
        val = -((65535 - val) + 1);
    return val;
}

void* read_mpu6050(void* arg) {
    //------------------------------------------------------------------------
    // Top Vars
    //------------------------------------------------------------------------
    int config_sleep_ms = 200; // default
    int file, MPU6050, MPU6050scanMilliseconds;
    pid_t t_pid = syscall(SYS_gettid);
    const char *bus = "/dev/i2c-1";
    //------------------------------------------------------------------------
    // Get/Set settings
    //------------------------------------------------------------------------
    pthread_mutex_lock(&rpiNode.lock);
        if (namevalue_find_as_integer(rpiNode.internal_config, "MPU6050", &MPU6050) == 0)
            MPU6050 = 0;
        if (namevalue_find_as_integer(rpiNode.internal_config, "MPU6050scanMilliseconds", &MPU6050scanMilliseconds) == 0)
            MPU6050scanMilliseconds = 200;
        // Exit if not flagged
        if (MPU6050 != 1) {
            pthread_mutex_unlock(&rpiNode.lock);
            return NULL;
        }
        // set sleep milliseconds
        if (MPU6050scanMilliseconds > 20)
            config_sleep_ms = MPU6050scanMilliseconds;
    pthread_mutex_unlock(&rpiNode.lock);
    //------------------------------------------------------------------------
    // Start / (notification)
    //------------------------------------------------------------------------
    printf("[MPU6050][%d]: Started -> reading every (%i)ms\n", t_pid, config_sleep_ms);
    //------------------------------------------------------------------------
    // Open i2c and connect to sensor
    //------------------------------------------------------------------------
    if ((file = open(bus, O_RDWR)) < 0) {
        fprintf(stderr, "[MPU6050][%d]: Failed to open the i2c bus: %s\n", t_pid, strerror(errno));
        exit(1);
    }
    
    if (ioctl(file, I2C_SLAVE, MPU_ADDR) < 0) {
        fprintf(stderr, "[MPU6050][%d]: Failed to connect to the sensor: %s\n", t_pid, strerror(errno));
        exit(1);
    }
    //------------------------------------------------------------------------
    // Set MPU6050 values_count (8) 
    //------------------------------------------------------------------------
    pthread_mutex_lock(&rpiNode.lock);
        rpiNode.internal.MPU6050.values_count = 8;
    pthread_mutex_unlock(&rpiNode.lock);
    //------------------------------------------------------------------------
    // Vars needed
    //------------------------------------------------------------------------
    float accel_x_buffer[WINDOW_SIZE] = {0};
    float accel_y_buffer[WINDOW_SIZE] = {0};
    float accel_z_buffer[WINDOW_SIZE] = {0};

    int accel_x_index = 0;
    int accel_y_index = 0;
    int accel_z_index = 0;
    //------------------------------------------------------------------------
    // Wake up MPU6050
    //------------------------------------------------------------------------
    char config[2] = {0x6B, 0};
    write(file, config, 2);
    //------------------------------------------------------------------------
    // Loop forever
    //------------------------------------------------------------------------
    while (1) {
        pthread_mutex_lock(&lock);
            if (!running) {
                pthread_mutex_unlock(&lock);
                break;
            }
        pthread_mutex_unlock(&lock);
        //--------------------------------------------------------------------
        // Read 14 bytes from register 0x3B
        //--------------------------------------------------------------------
        char reg = 0x3B;
        char data[14];
        write(file, &reg, 1);
        if (read(file, data, 14) != 14) {
            printf("[MPU6050][%d]: Error reading sensor data\n", t_pid);
            continue;
        }
        //--------------------------------------------------------------------
        // Accelerometer raw
        //--------------------------------------------------------------------
        int acc_x = read_word_2c(data[0], data[1]);
        int acc_y = read_word_2c(data[2], data[3]);
        int acc_z = read_word_2c(data[4], data[5]);
        //--------------------------------------------------------------------
        // Gyroscope (°/s)
        //--------------------------------------------------------------------
        float gyro_x = read_word_2c(data[8], data[9]) / 131.0;
        float gyro_y = read_word_2c(data[10], data[11]) / 131.0;
        float gyro_z = read_word_2c(data[12], data[13]) / 131.0;

        //--------------------------------------------------------------------
        // Accelerometer (g) smoothed 
        //--------------------------------------------------------------------
        float pitch, roll;
        float smoothed_x = smoothing_moving_average(acc_x, accel_x_buffer, &accel_x_index) / 16384.0;
        float smoothed_y = smoothing_moving_average(acc_y, accel_y_buffer, &accel_y_index) / 16384.0;
        float smoothed_z = smoothing_moving_average(acc_z, accel_z_buffer, &accel_z_index) / 16384.0;
        //--------------------------------------------------------------------
        // Accelerometer (radians)
        //--------------------------------------------------------------------
        calculate_angles(smoothed_x, smoothed_y, smoothed_z, &pitch, &roll);
        //--------------------------------------------------------------------
        //Write to values structure
        //--------------------------------------------------------------------
        mpu6050_assign_values(0, "pitch", pitch);
        mpu6050_assign_values(1, "roll", roll);
        mpu6050_assign_values(2, "accel-x", smoothed_x);
        mpu6050_assign_values(3, "accel-y", smoothed_y);
        mpu6050_assign_values(4, "accel-z", smoothed_z);
        mpu6050_assign_values(5, "gyro-x", gyro_x);
        mpu6050_assign_values(6, "gyro-y", gyro_y);
        mpu6050_assign_values(7, "gyro-z", gyro_z);

        sleep_ms(config_sleep_ms); 

    }
    //------------------------------------------------------------------------
    // Finished
    //------------------------------------------------------------------------
    close(file);
    printf("[MPU6050][%d]: Stopped\n", t_pid);

    return NULL;

}
