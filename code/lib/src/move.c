#include "move.h"

/**
 * Default move speed, follows PWM duty cycle requirements
 */
#define MOVE_SPEED 1000

char MoveInit(void) {
    // Initialise motors
    return MotorsInit();
}

char MoveForward(void) {
    return MotorsSetSpeed(MOVE_SPEED, MOVE_SPEED);
}

char MoveBackward(void) {
    return MotorsSetSpeed(-MOVE_SPEED, -MOVE_SPEED);
}

char MoveTankTurnL(void) {
    return MotorsSetSpeed(-MOVE_SPEED, MOVE_SPEED);
}

char MoveTankTurnR(void) {
    return MotorsSetSpeed(MOVE_SPEED, -MOVE_SPEED);
}

char MoveStop(void) {
    return MotorsSetSpeed(0, 0);
}

#ifdef TEST_MOVE
/*
 * Test of movement library. The robot moves forward -> backward -> tank turn
 * left -> tank turn right -> stop. The time between each action is set with
 * WAIT_TIME. The robot should end roughly where it started.
 */

#include <stdio.h>

#include "timers.h"

/** Time between changing direction */
#define WAIT_TIME 5000

/**
 * Block code execution for certain number of milliseconds. TIMERS library
 * is required.
 * 
 * @param _ms Number of milliseconds to wait
 */
void wait(unsigned int _ms) {
    // Get next desired time
    int next = TIMERS_GetTime() + _ms;
    // Wait until time is reached
    while (next > TIMERS_GetTime());
}

int main(void) {
    // Initialize subsystems, stopping on errors
    BOARD_Init();
    printf("TEST_MOVE, compiled on %s_%s\r\n", __DATE__, __TIME__);
    TIMERS_Init();
    if (PWM_Init() == ERROR) {
        return 1;
    }
    if (MotorsInit() == ERROR) {
        return 1;
    }
    
    if (MoveForward() == ERROR) return 1;
    wait(WAIT_TIME);
    
    if (MoveBackward() == ERROR) return 1;
    wait(WAIT_TIME);
    
    if (MoveTankTurnL() == ERROR) return 1;
    wait(WAIT_TIME);
    
    if (MoveTankTurnR() == ERROR) return 1;
    wait(WAIT_TIME);
    
    if (MoveStop() == ERROR) return 1;

    printf("TEST_MOVE Finished");

    while (1);

    return 0;
}

#endif /* TEST_MOVE */