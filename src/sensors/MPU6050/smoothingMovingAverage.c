#include "smoothingMovingAverage.h"

// Function definition for moving average
float smoothing_moving_average(float new_data, float* buffer, int* index) {

    buffer[*index] = new_data;  // Store the new data in the buffer
    *index = (*index + 1) % WINDOW_SIZE;  // Move to the next index, wrapping around

    // Calculate the average of the buffer
    float sum = 0.0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        sum += buffer[i];
    }

    return sum / WINDOW_SIZE;
    
}
