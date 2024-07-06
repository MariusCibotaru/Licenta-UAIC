#include "DistanceSensor.h"
#include <Wire.h>

VL53L0X sensor;
float distance = 0.0;

void setupDistanceSensor() {
  sensor.setTimeout(500);

  if (!sensor.init()) {
    Serial.println("Failed to detect and initialize VL53L0X!");
    while (1);
  }

  Serial.println("VL53L0X Found!");
  sensor.startContinuous();
}

void updateDistanceSensor() {
  float newDistance = sensor.readRangeContinuousMillimeters() / 10.0; // Преобразуем в сантиметры

  if (sensor.timeoutOccurred()) {
    Serial.print(" VL53L0X Timeout!");
  } else {
    // Обновляем расстояние только если оно больше 4 см
    if (newDistance > 4.0) { // Устанавливаем порог для минимального значения
      distance = newDistance;
    }
  }

}
