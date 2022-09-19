#ifndef SERVO_H
#define SERVO_H

#include "pwm.h"
#include "RC_Servo.h"

#define SERVO_PIN RC_PORTV04

#define RELEASE 2000
#define LOCK 1000

void ServoInit(void);

void ServoMove(unsigned short location);

#endif
