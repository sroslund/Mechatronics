#include "servo.h"

void ServoInit(void){
    RC_Init();
    RC_AddPins(SERVO_PIN);
}

void ServoMove(unsigned short location){
    RC_SetPulseTime(SERVO_PIN,location);
}