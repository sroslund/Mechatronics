/*
 Header file for tape sensors
 * 
 * NOTES:
 * BEACON_CHANGED must be a defined event in the config file
 * Update POST to cause the beacon to post to the desired service
 * REMEMBER TO CALL AD_INIT()!
 * Header guards cause problems if not changed
 */

#ifndef TAPE_H
#define TAPE_H

#include "AD.h"
#include "ES_Configure.h"
#include "ES_Events.h"
#include "Sensor_Config.h"
#include <stdio.h>

#define TAPE_UPPER_THRESHOLD 400
#define TAPE_LOWER_THRESHOLD 200

#define SIDE_UPPER_THRESHOLD 400
#define SIDE_LOWER_THRESHOLD 200

#define SIDE_TAPE_PIN AD_PORTV6
#define SIDE_TAPE 0x1

#define FR_TAPE_PIN AD_PORTW3
#define FR_TAPE 0x2

#define FL_TAPE_PIN AD_PORTW6
#define FL_TAPE 0x4

#define B1_TAPE_PIN AD_PORTW5
#define B1_TAPE 0x8


void TapeInit();

uint8_t TapeCheckEvent();

uint8_t SideCheckEvent();

uint16_t SideQuery();

uint16_t TapeQuery();

#endif
