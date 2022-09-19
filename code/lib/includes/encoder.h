#ifndef ENCODER_H
#define ENCODER_H

/**
 * @file encoder.h
 * @author John Madden (jmadden173@pm.me)
 * @date 2022-05-16
 * 
 * Encoder library to read QEI signal attached to DC motors. Supports two motors
 * for differential drive (left, right).
 * 
 * PIC Hardware Used:
 * - Timer3
 */

#include <xc.h>
#include <sys/attribs.h>

#include "BOARD.h"
#include "IO_Ports.h"

#define ENCODER_LEFT_A_PORT PORTY
#define ENCODER_LEFT_A_PIN PIN5
#define ENCODER_LEFT_B_PORT PORTY
#define ENCODER_LEFT_B_PIN PIN6

#define ENCODER_RIGHT_A_PORT PORTY
#define ENCODER_RIGHT_A_PIN PIN3
#define ENCODER_RIGHT_B_PORT PORTY
#define ENCODER_RIGHT_B_PIN PIN4

/**
 * Initialize encoder library
 * 
 * @return SUCCESS if initialized, ERROR otherwise
 */
char EncoderInit(void);

/**
 * Reset encoder counts to zero
 */
void EncoderResetCounts(void);

/**
 * Getter for left counts
 * 
 * @return Left counts
 */
int EncoderGetLeftCounts(void);

/**
 * Getter for right counts
 * 
 * @return Right counts
 */
int EncoderGetRightCounts(void);

/**
 * Get left encoder reading in degrees
 * 
 * @return Degrees rotated by left encoder
 */
double EncoderGetLeftDegrees(void);

/**
 * Get right encoder reading in degrees
 * 
 * @return Degrees rotated by right encoder
 */
double EncoderGetRightDegrees(void);

/**
 * Getter for left period
 * 
 * @return Left period
 */
unsigned int EncoderGetLeftPeriod(void);

/**
 * Getter for right period
 * 
 * @return Right period
 */
unsigned int EncoderGetRightPeriod(void);

#endif /* ENCODER_H */