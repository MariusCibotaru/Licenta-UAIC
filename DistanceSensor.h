#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include <VL53L0X.h>

extern float distance;

void setupDistanceSensor();
void updateDistanceSensor();

#endif // DISTANCE_SENSOR_H
