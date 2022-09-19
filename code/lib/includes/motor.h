#ifndef MOTOR_H
#define MOTOR_H

/**
 * @file motor.h
 * @author John Madden
 * @date 2022-05-13
 * 
 * Low level motor driver code.
 * 
 * Required libraries to be initialized
 * - PWM
 */

#include "pwm.h"
#include "IO_Ports.h"
#include <stdlib.h>

// Left motor pins
#define LEFT_MOTOR_ENABLE PWM_PORTY10
#define LEFT_MOTOR_IN1_PORT PORTY
#define LEFT_MOTOR_IN1_PIN PIN7
#define LEFT_MOTOR_IN2_PORT PORTY
#define LEFT_MOTOR_IN2_PIN PIN8

// Right motor pins
#define RIGHT_MOTOR_ENABLE PWM_PORTY12
#define RIGHT_MOTOR_IN2_PORT PORTY
#define RIGHT_MOTOR_IN2_PIN PIN9
#define RIGHT_MOTOR_IN1_PORT PORTY
#define RIGHT_MOTOR_IN1_PIN PIN11

/**
 * @brief Initialize the motor library
 * 
 * Set direction of IO Pins for motor and initializes the PWM library.
 * 
 * @return SUCCESS if Motors was initialized, ERROR otherwise
 */
char MotorsInit(void);

/**
 * Sets the speed of both motors
 * 
 * @param _left Left motor speed
 * @param _right Right motor speed
 * @return SUCCESS if speed was set, ERROR otherwise
 */
char MotorsSetSpeed(short _left, short _right);

/**
 * Sets the speed of the left motor
 * 
 * @param speed New speed
 */
char MotorsSetLeftSpeed(int speed);

/**
 * Sets the speed of the right motor
 * 
 * @param speed New speed
 */
char MotorsSetRightSpeed(int speed);

/**
 * Gets the left motor speed
 * 
 * @return Left motor speed
 */
int MotorsGetLeftSpeed(void);

/**
 * Gets the right motor speed
 * 
 * @return Right motor speed
 */
int MotorsGetRightSpeed(void);

#endif