#include "Pilot.h"
#include "MotorControl.h"
#include <Arduino.h>

const float movementSpeed = 10.0;
const float rotationSpeed = 10.0;
extern bool pilotOverride;

void setupPilot() {
  pilotOverride = false;
}

void moveForward() {
  pilotOverride = true;
  Serial.println("Moving forward");
  controlTorque(150.0, 0.0, movementSpeed, 0.0);
}

void moveBackward() {
  pilotOverride = true;
  Serial.println("Moving backward");
  controlTorque(150.0, 0.0, -movementSpeed, 0.0);
}

void moveLeft() {
  pilotOverride = true;
  Serial.println("Moving left");
  controlTorque(150.0, 0.0, 0.0, movementSpeed);

}

void moveRight() {
  pilotOverride = true;
  Serial.println("Moving right");
  controlTorque(150.0, 0.0, 0.0, -movementSpeed);
}

void rotateLeft() {
  pilotOverride = true;
  Serial.println("Rotating left");
   controlTorque(150.0, rotationSpeed, 0.0, 0.0);

}

void rotateRight() {
  pilotOverride = true;
  Serial.println("Rotating right");
  controlTorque(150.0, -rotationSpeed, 0.0, 0.0);

}

void stopMovement() {
  pilotOverride = false;
  Serial.println("Stopping movement");
  controlTorque(150.0, 0.0, 0.0, 0.0);
}

void controlTorque(float thrustTarget, float torqueX, float torqueY, float torqueZ) {
  float motor1 = constrain(thrustTarget + torqueY + torqueX - torqueZ, 0, 255);
  float motor2 = constrain(thrustTarget + torqueY - torqueX + torqueZ, 0, 255);
  float motor3 = constrain(thrustTarget - torqueY + torqueX + torqueZ, 0, 255);
  float motor4 = constrain(thrustTarget - torqueY - torqueX - torqueZ, 0, 255);

  setMotorSpeed(1, motor1);
  setMotorSpeed(2, motor2);
  setMotorSpeed(3, motor3);
  setMotorSpeed(4, motor4);

  Serial.print("Motor speeds: ");
  Serial.print(motor1);
  Serial.print(", ");
  Serial.print(motor2);
  Serial.print(", ");
  Serial.print(motor3);
  Serial.print(", ");
  Serial.println(motor4);
}
