#include "MotorControl.h"
#include <Arduino.h>

#define MOTOR_PIN1 5
#define MOTOR_PIN2 18
#define MOTOR_PIN3 19
#define MOTOR_PIN4 21

extern WebServer server;
extern int motorSpeeds[4]; 

void setupMotors() {
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(MOTOR_PIN3, OUTPUT);
  pinMode(MOTOR_PIN4, OUTPUT);
}

void setMotorSpeed(int motor, int speed) {
  switch (motor) {
    case 1:
      analogWrite(MOTOR_PIN1, speed);
      motorSpeeds[0] = speed;
      break;
    case 2:
      analogWrite(MOTOR_PIN2, speed);
      motorSpeeds[1] = speed;
      break;
    case 3:
      analogWrite(MOTOR_PIN3, speed);
      motorSpeeds[2] = speed;
      break;
    case 4:
      analogWrite(MOTOR_PIN4, speed);
      motorSpeeds[3] = speed;
      break;
    default:
      break;
  }
}

int getMotorSpeed(int motor) {
  switch (motor) {
    case 1:
      return motorSpeeds[0];
    case 2:
      return motorSpeeds[1];
    case 3:
      return motorSpeeds[2];
    case 4:
      return motorSpeeds[3];
    default:
      return 0;
  }
}

void handleMotor() {
  if (server.hasArg("motor") && server.hasArg("speed")) {
    int motor = server.arg("motor").toInt();
    int speed = server.arg("speed").toInt();
    setMotorSpeed(motor, speed);
    server.send(200, "text/plain", "Скорость мотора обновлена");
  } else {
    server.send(400, "text/plain", "Отсутствует параметр мотор или скорость");
  }
}

void handleAllMotors() {
  if (server.hasArg("speed")) {
    int speed = server.arg("speed").toInt();
    setMotorSpeed(1, speed);
    setMotorSpeed(2, speed);
    setMotorSpeed(3, speed);
    setMotorSpeed(4, speed);
    server.send(200, "text/plain", "Скорость всех моторов обновлена");
  } else {
    server.send(400, "text/plain", "Отсутствует параметр скорость");
  }
}
