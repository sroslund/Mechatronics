#include "beacon.h"
#include <stdio.h>

static uint16_t lastBeacon = 0;

void BeaconInit(){
    //printf("inited");
    IO_PortsSetPortInputs(BEACON_INPUT_PORT, BEACON_INPUT_PIN);
    //PORTY03_TRIS = 1;
}

uint8_t BeaconCheckEvent(){
        
    
    uint16_t currBeacon = IO_PortsReadPort(BEACON_INPUT_PORT)&BEACON_INPUT_PIN;
    
    //printf("Beacon: %d",PORTY03_BIT);
    
    if(currBeacon != lastBeacon){
        //printf("BEACON CHECK");
        lastBeacon = currBeacon;
        ES_Event Beacon;
        Beacon.EventType = BEACON_CHANGE;
        if(currBeacon){
            Beacon.EventParam = 0;
        } else {
           Beacon.EventParam = 1;
        }
        POST_BEACON(Beacon);
        return 1;
    }
    return 0;
}

uint16_t BeaconQuery(){
    if((IO_PortsReadPort(BEACON_INPUT_PORT)&BEACON_INPUT_PIN)){
        //printf("returning zero");
        return 0;
    }
    //printf("returning one");
    return 1;
}