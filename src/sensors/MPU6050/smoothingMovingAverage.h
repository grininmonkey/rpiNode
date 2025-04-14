#ifndef MPU6050_SMOOTHING_MOVING_AVERAGE_H
#define MPU6050_SMOOTHING_MOVING_AVERAGE_H

#define WINDOW_SIZE 10  // Define the window size for the moving average

// Function declaration
float smoothing_moving_average(float new_data, float* buffer, int* index);

#endif  