#include "tape.h"

static uint16_t lastTape = 0;

static uint16_t lastSide = 0;

void TapeInit(){
    AD_AddPins(B1_TAPE_PIN | FL_TAPE_PIN | FR_TAPE_PIN | SIDE_TAPE_PIN);
}

uint8_t TapeCheckEvent(){
    
    static int cycleCount = 0;
    if(cycleCount < 15000){
        cycleCount++;
        return 0;
    }
    
    uint16_t currTape = lastTape;
    
    // middle reads back, front left is last,
    //.printf("FrontL: %d FrontR: %d Back: %d \r\n",AD_ReadADPin(FL_TAPE_PIN),AD_ReadADPin(FR_TAPE_PIN),AD_ReadADPin(B1_TAPE_PIN));
    
    if(AD_ReadADPin(FR_TAPE_PIN) > TAPE_UPPER_THRESHOLD && !(lastTape & FR_TAPE)){
        currTape ^= FR_TAPE;
    } else if(AD_ReadADPin(FR_TAPE_PIN) < TAPE_LOWER_THRESHOLD && (lastTape & FR_TAPE)){
        currTape ^= FR_TAPE;
    }
    
    if(AD_ReadADPin(FL_TAPE_PIN) > TAPE_UPPER_THRESHOLD && !(lastTape & FL_TAPE)){
        currTape ^= FL_TAPE;
    } else if(AD_ReadADPin(FL_TAPE_PIN) < TAPE_LOWER_THRESHOLD && (lastTape & FL_TAPE)){
        currTape ^= FL_TAPE;
    }
    
    if(AD_ReadADPin(B1_TAPE_PIN) > TAPE_UPPER_THRESHOLD && !(lastTape & B1_TAPE)){
        currTape ^= B1_TAPE;
    } else if(AD_ReadADPin(B1_TAPE_PIN) < TAPE_LOWER_THRESHOLD && (lastTape & B1_TAPE)){
        currTape ^= B1_TAPE;
    }
    
    if(currTape != lastTape){
        //printf("Posting an event \n\r");
        cycleCount = 0;
        lastTape = currTape;
        ES_Event Tape;
        Tape.EventType = TAPE_CHANGE;
        Tape.EventParam = (uint16_t) currTape;
        POST_TAPE(Tape);
        return 1;
    }
    return 0;
}

uint8_t SideCheckEvent(){
    //printf("AD: %d \n\r",AD_ReadADPin(SIDE_TAPE_PIN));
    uint16_t currTape = lastSide;
    
    if(AD_ReadADPin(SIDE_TAPE_PIN) > SIDE_UPPER_THRESHOLD && !(lastSide & SIDE_TAPE)){
        //printf("I am toggling it because it is high \n");
        currTape ^= SIDE_TAPE;
    } else if(AD_ReadADPin(SIDE_TAPE_PIN) < SIDE_LOWER_THRESHOLD && (lastSide & SIDE_TAPE)){
        //printf("I am toggling it because it is low \n");
        currTape ^= SIDE_TAPE;
    }
   //printf("CurrTape: %d, LastTape: %d \n\r\n",currTape,lastSide);
    
    if(currTape != lastSide){
        //printf("TAPE CHECK");
        lastSide = currTape;
        ES_Event Tape;
        Tape.EventType = SIDE_CHANGE;
        Tape.EventParam = (uint16_t) currTape;
        POST_TAPE(Tape);
        return 1;
    }
    return 0;
}

uint16_t SideQuery(){
    uint16_t val = AD_ReadADPin(SIDE_TAPE_PIN);
    //printf("got: %d \n\r",val);
    return AD_ReadADPin(SIDE_TAPE_PIN);
}

uint16_t TapeQuery(){
    uint16_t currTape = lastTape;
    
    // middle reads back, front left is last,
    //.printf("FrontL: %d FrontR: %d Back: %d \r\n",AD_ReadADPin(FL_TAPE_PIN),AD_ReadADPin(FR_TAPE_PIN),AD_ReadADPin(B1_TAPE_PIN));
    
    if(AD_ReadADPin(FR_TAPE_PIN) > TAPE_UPPER_THRESHOLD && !(lastTape & FR_TAPE)){
        currTape ^= FR_TAPE;
    } else if(AD_ReadADPin(FR_TAPE_PIN) < TAPE_LOWER_THRESHOLD && (lastTape & FR_TAPE)){
        currTape ^= FR_TAPE;
    }
    
    if(AD_ReadADPin(FL_TAPE_PIN) > TAPE_UPPER_THRESHOLD && !(lastTape & FL_TAPE)){
        currTape ^= FL_TAPE;
    } else if(AD_ReadADPin(FL_TAPE_PIN) < TAPE_LOWER_THRESHOLD && (lastTape & FL_TAPE)){
        currTape ^= FL_TAPE;
    }
    
    if(AD_ReadADPin(B1_TAPE_PIN) > TAPE_UPPER_THRESHOLD && !(lastTape & B1_TAPE)){
        currTape ^= B1_TAPE;
    } else if(AD_ReadADPin(B1_TAPE_PIN) < TAPE_LOWER_THRESHOLD && (lastTape & B1_TAPE)){
        currTape ^= B1_TAPE;
    }
    return currTape;
}
