#include "motor.h"

char MotorsInit(void) {
    // Setup IO pins
    IO_PortsSetPortOutputs(LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN);
    IO_PortsSetPortOutputs(LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN);
    IO_PortsSetPortOutputs(RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN);
    IO_PortsSetPortOutputs(RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN);

    PWM_SetFrequency(MIN_PWM_FREQ);
    // Initialize PWM library
    return PWM_AddPins(LEFT_MOTOR_ENABLE | RIGHT_MOTOR_ENABLE);
}

char MotorsSetSpeed(short _left, short _right) {
    // return code
    char rc;

    // set left speed
    rc = MotorsSetLeftSpeed(_left);
    if (rc == ERROR) {
        return rc;
    }

    // set right speed
    rc = MotorsSetRightSpeed(_right);

    return rc;
}

char MotorsSetLeftSpeed(int speed) {
    // Forward
    if (speed > 0) {
        IO_PortsSetPortBits(LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN);
        IO_PortsClearPortBits(LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN);
        // Backward
    } else {
        IO_PortsSetPortBits(LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN);
        IO_PortsClearPortBits(LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN);
    }

    // Set motor speed
    return PWM_SetDutyCycle(LEFT_MOTOR_ENABLE, abs(speed));
}

char MotorsSetRightSpeed(int speed) {
    // Forward
    if (speed > 0) {
        IO_PortsSetPortBits(RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN);
        IO_PortsClearPortBits(RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN);
        // Backward
    } else {
        IO_PortsSetPortBits(RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN);
        IO_PortsClearPortBits(RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN);
    }

    // Set motor speed
    return PWM_SetDutyCycle(RIGHT_MOTOR_ENABLE, abs(speed));
}

int MotorsGetLeftSpeed(void) {
    return PWM_GetDutyCycle(RIGHT_MOTOR_ENABLE);
}

int MotorGetRightSpeed(void) {
    return PWM_GetDutyCycle(LEFT_MOTOR_ENABLE);
}

#ifdef TEST_MOTOR
/*
 * Test of motor driver functions. Connect motors to pins defined in "motor.h".
 * The left ramps from stopped -> max speed -> stopped over time in 20% steps.
 * The sequence is then repeated for the right motor.
 */

#include <stdio.h>

#include "timers.h"

/** Time between changing motor speeds */
#define WAIT_TIME 2000

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
    printf("TEST_MOTOR, compiled on %s_%s\r\n", __DATE__, __TIME__);
    TIMERS_Init();
    if (PWM_Init() == ERROR) {
        return 1;
    }
    if (MotorsInit() == ERROR) {
        return 1;
    }

    // Ramp of motor speeds, adjust as needed
    const short ramp_arr[] = {0, 200, 400, 600, 800, 1000, 800, 600, 400, 200, 0};
    const int ramp_len = 11;

    // Ramp idx
    int i;

    for (i = 0; i < ramp_len; i++) {
        // Update motor speed checking for errors
        if (MotorsSetLeftSpeed(ramp_arr[i]) == ERROR) {
            return 1;
        }

        printf("Left Motor Speed: %d\r\n", ramp_arr[i]);

        wait(WAIT_TIME);
    }

    for (i = 0; i < ramp_len; i++) {
        // Update motor speed checking for errors
        if (MotorsSetRightSpeed(ramp_arr[i]) == ERROR) {
            return 1;
        }

        printf("Right Motor Speed: %d\r\n", ramp_arr[i]);

        wait(WAIT_TIME);
    }
    
    printf("TEST_MOTORS Finished");

    // Spin
    while (1);

    return 0;
}

#endif /* TEST_MOTOR */