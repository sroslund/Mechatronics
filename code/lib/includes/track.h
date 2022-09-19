/*
 Header file for track sensors
 * 
 * NOTES:
 * BEACON_CHANGED must be a defined event in the config file
 * Update POST to cause the beacon to post to the desired service
 * REMEMBER TO CALL AD_INIT()!
 */

#ifndef TRACK_H
#define TRACK_H

#include "AD.h"
#include "ES_Configure.h"
#include "ES_Events.h"
#include "Sensor_Config.h"

#define UPPER_THRESHOLD 860
#define LOWER_THRESHOLD 400

#define TRACK_FORWARD_PIN AD_PORTV8
#define TRACK_BACK_PIN AD_PORTV7

#define FRONT_TRACK 0x1
#define BACK_TRACK 0x2

void TrackInit();

uint8_t TrackCheckEvent();

uint16_t QueryFrontTrack();

uint16_t QueryBackTrack();

#endif
