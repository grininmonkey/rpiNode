#ifndef MPU6050_CALCULATE_AGNLES_H
#define MPU6050_CALCULATE_AGNLES_H

// Declaration of the function that calculates pitch and roll
void calculate_angles(float ax, float ay, float az, float* pitch, float* roll);

#endif // ACCELEROMETER_H