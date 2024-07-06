#ifndef AUTOPILOT_H
#define AUTOPILOT_H

void setupAutopilot();
void updateAutopilot();
void handleTakeoff();
void handleCancelTakeoff();
void handleSetTargetHeight();
float computePID(float kp, float ki, float kd, float target, float current, float &previousError, float &integral);
void adjustAltitude();

extern float targetAltitude;  
extern float desiredAltitude;  
extern float desiredRoll;  
extern float desiredPitch;  
extern bool takeoffCommand;

#endif // AUTOPILOT_H
