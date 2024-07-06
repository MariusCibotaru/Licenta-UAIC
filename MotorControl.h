#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <WebServer.h>

void setupMotors();
void setMotorSpeed(int motor, int speed);
void handleMotor();
void handleAllMotors();
void updateMotorSpeeds(int motor, int speed);
int getMotorSpeed(int motor);

extern int motorSpeeds[4];

#endif // MOTOR_CONTROL_H
