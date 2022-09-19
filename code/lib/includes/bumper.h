/*
 Header file for bumper sensors
 * 
 * NOTES:
 * BEACON_CHANGED must be a defined event in the config file
 * Update POST to cause the beacon to post to the desired service
 */

#ifndef BUMPER_H
#define BUMPER_H

#include "IO_Ports.h"
#include "ES_Configure.h"
#include "ES_Events.h"
#include "Sensor_Config.h"

#define DEBOUNCE_CONSTANT 100

#define BUMPERFR_INPUT_PORT PORTZ
#define BUMPERFR_INPUT_PIN PIN11
#define BUMPERFL_INPUT_PORT PORTZ
#define BUMPERFL_INPUT_PIN PIN6

#define BUMPERLB_INPUT_PORT PORTZ
#define BUMPERLB_INPUT_PIN PIN3
#define BUMPERLF_INPUT_PORT PORTZ
#define BUMPERLF_INPUT_PIN PIN5

#define BUMPERRB_INPUT_PORT PORTZ
#define BUMPERRB_INPUT_PIN PIN7
#define BUMPERRF_INPUT_PORT PORTZ
#define BUMPERRF_INPUT_PIN PIN8

#define BUMPERBR_INPUT_PORT PORTZ
#define BUMPERBR_INPUT_PIN PIN12
#define BUMPERBL_INPUT_PORT PORTZ
#define BUMPERBL_INPUT_PIN PIN4

void BumperInit();

uint8_t BumperCheckEvent();

uint16_t BumperQuery(void);

#endif
