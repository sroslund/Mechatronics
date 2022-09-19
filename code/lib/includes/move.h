#ifndef MOVE_H
#define MOVE_H

/**
 * @file move.h
 * @author John Madden (jmadden173@pm.me)
 * @date 2022-05-14
 * 
 * Implements a abstraction layer to transfer between movement functions to
 * motor speed functions.
 */

#include "motor.h"

/**
 * @brief Initializes the move library
 * 
 * Initializes the following libraries
 * - Motors
 * 
 * @return SUCCESS if Move was initialized, ERROR otherwise
 */
char MoveInit(void);

/**
 * @brief Move forward
 * 
 * @return SUCCESS if motor speeds are correctly set, ERROR otherwise
 */
char MoveForward(void);

/**
 * @brief Move backward
 * 
 * @return SUCCESS if motor speeds are correctly set, ERROR otherwise
 */
char MoveBackward(void);

/**
 * @brief Tank turn left
 * 
 * @return SUCCESS if motor speeds are correctly set, ERROR otherwise
 */
char MoveTankTurnL(void);

/**
 * @brief Tank turn right
 * 
 * @return SUCCESS if motor speeds are correctly set, ERROR otherwise
 */
char MoveTankTurnR(void);

/**
 * @brief Stop moving
 * 
 * @return SUCCESS if motors are stopped, ERROR otherwise
 */
char MoveStop(void);

#endif /* MOVE_H */