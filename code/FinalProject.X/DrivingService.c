/*
 * File: TemplateService.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a simple service to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "BOARD.h"
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "DrivingService.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

/**
 * @brief Convert degrees to mm
 * 
 * (DIAMETER) * PI
 */
#define CONV_DEG_MM 226.195

/** Distance between wheels in mm */
#define AXLE_DIAMETER 211.074

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/
/* You will need MyPriority and maybe a state variable; you may need others
 * as well. */

static uint8_t MyPriority;

/** Pose of robot */
typedef struct {
    double x;
    double y;
    double theta;
} Pose_t;

/** Current pose */
static Pose_t Pose;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitDrivingService(uint8_t Priority) {
    ES_Event ThisEvent;

    MyPriority = Priority;

    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.

    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostDrivingService(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunDrivingService(ES_Event ThisEvent) {
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    switch (ThisEvent.EventType) {
        case ES_INIT:
            // No hardware initialization or single time setups, those
            // go in the init function above.
            //
            // This section is used to reset service for some reason
            
            // Reset pose
            Pose.x = 0;
            Pose.y = 0;
            Pose.theta = 0;
            
            // Start timer
            ES_Timer_InitTimer(DRIVING_SERVICE_TIMER, DRIVING_SERVICE_DT);
            
            break;
            
        case ES_TIMEOUT:
            // Check for correct timer
            if (ThisEvent.EventParam == DRIVING_SERVICE_TIMER) {
                break;
            }
            
            // Convert rotary encoder count to m/s
            // degrees/dt * m/deg
            
            // Calculate wheel speed
            double dsl = EncoderGetLeftDegrees() * CONV_DEG_MM * DRIVING_SERVICE_DT / 1000.;
            double dsr = EncoderGetRightDegrees() * CONV_DEG_MM * DRIVING_SERVICE_DT / 1000.;
            double ds = (dsl + dsr) / 2;
            
            // Update pose
            Pose.x += ds * cos(Pose.theta);
            Pose.y += ds * sin(Pose.theta);
            Pose.theta += (dsr - dsl) / AXLE_DIAMETER;
            
#ifdef PRINT_POSE
            printf("x: %f, y: %f, theta: %f\r\n", Pose.x, Pose.y, Pose.theta);
#endif /* PRINT_POSE */
            
            // Reset timer
            ES_Timer_InitTimer(DRIVING_SERVICE_TIMER, DRIVING_SERVICE_DT);
            // Reset encoder
            EncoderResetCounts();
            
            break;
            
        default:
            break;
    }

    return ReturnEvent;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/

