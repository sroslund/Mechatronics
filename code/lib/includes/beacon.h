/*
 Header file for beacon detector sensor
 * 
 * NOTES:
 * BEACON_CHANGED must be a defined event in the config file
 * Update POST to cause the beacon to post to the desired service
 */

#ifndef BEACON_H
#define BEACON_H

#include "IO_Ports.h"
#include "xc.h"
#include "ES_Configure.h"
#include "ES_Events.h"
#include "Sensor_Config.h"

#define BEACON_INPUT_PORT PORTY
#define BEACON_INPUT_PIN PIN3

void BeaconInit();

uint8_t BeaconCheckEvent();

uint16_t BeaconQuery();

#endif
