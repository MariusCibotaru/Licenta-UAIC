#ifndef SENSOR_H
#define SENSOR_H

#include <Wire.h>
#include <MPU6050_light.h>

extern float accelX, accelY, accelZ;
extern float gyroX, gyroY, gyroZ;
extern float roll, pitch, yaw;
extern bool sensorError;

void setupSensors();
void updateSensors();

#endif // SENSOR_H
