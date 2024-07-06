#include "Autopilot.h"
#include "DistanceSensor.h"
#include "MotorControl.h"
#include "Sensor.h"
#include <Arduino.h>

float kp_altitude = 1.0;
float ki_altitude = 0.5;
float kd_altitude = 0.25;

float kp_angle = 1.0;
float ki_angle = 0.1;
float kd_angle = 0.05;

float targetAltitude = 0.0; 
float desiredAltitude = 0.0;
float desiredRoll = 0.0;
float desiredPitch = 0.0;
bool takeoffCommand = false;
bool pilotOverride = false;

float previousErrorAltitude = 0.0;
float integralAltitude = 0.0;

float previousErrorRoll = 0.0;
float integralRoll = 0.0;

float previousErrorPitch = 0.0;
float integralPitch = 0.0;

extern WebServer server;

void setupAutopilot() {
}

void updateAutopilot() {
  if (takeoffCommand && !pilotOverride) {
    adjustAltitude();

    float pidOutputAltitude = computePID(kp_altitude, ki_altitude, kd_altitude, targetAltitude, distance, previousErrorAltitude, integralAltitude);
    float pidOutputRoll = computePID(kp_angle, ki_angle, kd_angle, desiredRoll, -roll, previousErrorRoll, integralRoll);
    float pidOutputPitch = computePID(kp_angle, ki_angle, kd_angle, desiredPitch, -pitch, previousErrorPitch, integralPitch);

    float motor1 = constrain(pidOutputAltitude - pidOutputRoll + pidOutputPitch, 0, 255);
    float motor2 = constrain(pidOutputAltitude + pidOutputRoll + pidOutputPitch, 0, 255);
    float motor3 = constrain(pidOutputAltitude + pidOutputRoll - pidOutputPitch, 0, 255);
    float motor4 = constrain(pidOutputAltitude - pidOutputRoll - pidOutputPitch, 0, 255);

    motor1 = max(motor1, 127.0f);
    motor2 = max(motor2, 127.0f);
    motor3 = max(motor3, 127.0f);
    motor4 = max(motor4, 127.0f);

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

    if (abs(desiredAltitude - distance) <= 0.1 && desiredAltitude == 1.0) {
      takeoffCommand = false;
      targetAltitude = 0.0;
      setMotorSpeed(1, 0);
      setMotorSpeed(2, 0);
      setMotorSpeed(3, 0);
      setMotorSpeed(4, 0);
      Serial.println("Motors stopped: 0, 0, 0, 0");
      desiredAltitude = 0.0;
    }
  }
}

void adjustAltitude() {
  if (abs(desiredAltitude - targetAltitude) > 0.1) {
    if (desiredAltitude > targetAltitude) {
      targetAltitude += 0.1; 
    } else {
      targetAltitude -= 0.1; 
    }
  }
}

float computePID(float kp, float ki, float kd, float target, float current, float &previousError, float &integral) {
  float error = target - current;
  integral += error;
  float derivative = error - previousError;
  previousError = error;
  
  float output = kp * error + ki * integral + kd * derivative;
  return output;
}

void handleTakeoff() {
  Serial.println("Takeoff command received"); 
  desiredAltitude = 10.0; 
  takeoffCommand = true;
  server.send(200, "text/plain", "takeoff");
}

void handleCancelTakeoff() {
  Serial.println("Cancel takeoff command received");
  desiredAltitude = 1.0; 
  if (distance <= 4.0) {
    takeoffCommand = false;
    targetAltitude = 0.0;
    setMotorSpeed(1, 0);
    setMotorSpeed(2, 0);
    setMotorSpeed(3, 0);
    setMotorSpeed(4, 0);
    desiredAltitude = 0.0; 
    Serial.println("Motors stopped: 0, 0, 0, 0");
  } else {
    Serial.println("Descending...");
  }

  server.send(200, "text/plain", "cancel takeoff");
}

void handleSetTargetHeight() {
  if (server.hasArg("height")) {
    desiredAltitude = server.arg("height").toFloat();
    server.send(200, "text/plain", "Target height set");
  } else {
    server.send(400, "text/plain", "Missing height parameter");
  }
}
