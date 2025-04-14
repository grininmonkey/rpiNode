#include <math.h>  // For atan2() and sqrt()
#include <stdio.h>

// Function to calculate pitch and roll angles (in degrees) from accelerometer data
// expecting values in (g)s

void calculate_angles(float ax, float ay, float az, float* pitch, float* roll) {
    // Calculate the pitch angle (in radians)
    *pitch = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / M_PI;

    // Calculate the roll angle (in radians)
    *roll = atan2(-ax, az) * 180.0 / M_PI;
}
