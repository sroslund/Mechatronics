#include "track.h"
#include <stdio.h>

static uint16_t lastTrack = 0;

void TrackInit(){
    printf("Activate: %d",AD_AddPins(TRACK_FORWARD_PIN | TRACK_BACK_PIN));
}

uint8_t TrackCheckEvent(){
    uint16_t currTrack = lastTrack;
    static uint8_t counter = 0;
    
    unsigned F_read = AD_ReadADPin(TRACK_FORWARD_PIN);
    unsigned B_read = AD_ReadADPin(TRACK_BACK_PIN);
    
    //printf("F: %d \n\r",F_read);
    
    
    if(F_read > UPPER_THRESHOLD && !(lastTrack & FRONT_TRACK)){
        currTrack ^= FRONT_TRACK;
    } else if(F_read < LOWER_THRESHOLD && (lastTrack & FRONT_TRACK)){
        currTrack ^= FRONT_TRACK;
    }
    
    if(B_read > UPPER_THRESHOLD && !(lastTrack & BACK_TRACK)){
        //printf("T1, %d",lastTrack & BACK_TRACK);
        currTrack ^= BACK_TRACK;
        //printf("Toggle, %d",lastTrack & BACK_TRACK);
    } else if(B_read < LOWER_THRESHOLD && (lastTrack & BACK_TRACK)){
        //printf("T2");
        currTrack ^= BACK_TRACK;
    }
    
    if(currTrack != lastTrack){
        counter++;
    } else {
        counter = 0;
    }
    
    if(counter > 50){
        ES_Event Track;
        Track.EventType = TRACK_CHANGE;
        Track.EventParam = (uint16_t) currTrack;
        POST_TRACK(Track);
        lastTrack = currTrack;
        counter = 0;
        return 1;
    }
    return 0;
}

uint16_t QueryFrontTrack(){
    return AD_ReadADPin(TRACK_FORWARD_PIN);
}

uint16_t QueryBackTrack(){
    return AD_ReadADPin(TRACK_BACK_PIN);
}