#ifndef PILOT_H
#define PILOT_H

void setupPilot();
void moveForward();
void moveBackward();
void moveLeft();
void moveRight();
void rotateLeft();
void rotateRight();
void stopMovement();
void controlTorque(float thrustTarget, float torqueX, float torqueY, float torqueZ);

#endif // PILOT_H
