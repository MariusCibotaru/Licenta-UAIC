#include "Sensor.h"
#include <Wire.h>
#include <MPU6050_light.h>
#include <math.h>

MPU6050 mpu(Wire);

float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float roll, pitch, yaw;
float accelX_ms2 = 0.0;
bool sensorError = false;

float alpha = 0.95;

unsigned long timer = 0;
float dt;

void setupSensors() {
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU6050 initialization failed with code: ");
    Serial.println(status);
    sensorError = true;
    return;
  }

  Serial.println("MPU6050 Found!");
  mpu.calcOffsets(true, true);
  timer = millis(); 
}

void updateSensors() {
  if (sensorError) {
    return;
  }

  mpu.update();

  accelX = mpu.getAccX();
  accelY = mpu.getAccY();
  accelZ = mpu.getAccZ();
  accelX_ms2 = accelX * 9.81;

  gyroX = mpu.getGyroX();
  gyroY = mpu.getGyroY();
  gyroZ = mpu.getGyroZ();

  dt = (millis() - timer) / 1000.0;
  timer = millis();

  float accelRoll = atan2(accelY, accelZ) * 57.3;
  float accelPitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 57.3; 

  roll += gyroX * dt;
  pitch += gyroY * dt;
  yaw += gyroZ * dt;

  roll = alpha * roll + (1 - alpha) * accelRoll;
  pitch = alpha * pitch + (1 - alpha) * accelPitch;

  if (yaw > 180) {
    yaw -= 360;
  } else if (yaw < -180) {
    yaw += 360;
  }

}
