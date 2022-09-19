#include "bumper.h"
#include <stdio.h>

static uint16_t lastBumper = 0;
static uint16_t changedBumper = 0;

void BumperInit() {
    IO_PortsSetPortInputs(BUMPERFL_INPUT_PORT, 0xFFFF);
}

uint8_t BumperCheckEvent() {
    //printf("testing");
    static int cycleCount = 0;
    if(cycleCount < DEBOUNCE_CONSTANT){
        cycleCount++;
        return 0;
    }
    
    //printf("read = %d \n\r",IO_PortsReadPort(BUMPERFR_INPUT_PORT));
    
    uint16_t currBumper = 0;
    
    if (IO_PortsReadPort(BUMPERFR_INPUT_PORT) & BUMPERFR_INPUT_PIN) currBumper |= BUMPERFR_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERFL_INPUT_PORT) & BUMPERFL_INPUT_PIN) currBumper |= BUMPERFL_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERLF_INPUT_PORT) & BUMPERLF_INPUT_PIN) currBumper |= BUMPERLF_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERLB_INPUT_PORT) & BUMPERLB_INPUT_PIN) currBumper |= BUMPERLB_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERRF_INPUT_PORT) & BUMPERRF_INPUT_PIN) currBumper |= BUMPERRF_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERRB_INPUT_PORT) & BUMPERRB_INPUT_PIN) currBumper |= BUMPERRB_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERBR_INPUT_PORT) & BUMPERBR_INPUT_PIN) currBumper |= BUMPERBR_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERBL_INPUT_PORT) & BUMPERBL_INPUT_PIN) currBumper |= BUMPERBL_INPUT_PIN;
    

    if (currBumper != lastBumper) {
        lastBumper = currBumper;
        ES_Event Bumper;
        Bumper.EventType = BUMPER_CHANGE;
        Bumper.EventParam = (uint16_t) currBumper;
        POST_BUMPER(Bumper);
        cycleCount = 0;
        return 1;
    }
    return 0;
}

uint16_t BumperQuery(void){
    uint16_t currBumper = 0;
    
    if (IO_PortsReadPort(BUMPERFR_INPUT_PORT) & BUMPERFR_INPUT_PIN) currBumper |= BUMPERFR_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERFL_INPUT_PORT) & BUMPERFL_INPUT_PIN) currBumper |= BUMPERFL_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERLF_INPUT_PORT) & BUMPERLF_INPUT_PIN) currBumper |= BUMPERLF_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERLB_INPUT_PORT) & BUMPERLB_INPUT_PIN) currBumper |= BUMPERLB_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERRF_INPUT_PORT) & BUMPERRF_INPUT_PIN) currBumper |= BUMPERRF_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERRB_INPUT_PORT) & BUMPERRB_INPUT_PIN) currBumper |= BUMPERRB_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERBR_INPUT_PORT) & BUMPERBR_INPUT_PIN) currBumper |= BUMPERBR_INPUT_PIN;
    if (IO_PortsReadPort(BUMPERBL_INPUT_PORT) & BUMPERBL_INPUT_PIN) currBumper |= BUMPERBL_INPUT_PIN;
    return currBumper;
}
