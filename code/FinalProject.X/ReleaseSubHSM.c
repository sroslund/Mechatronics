/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "HSM.h"
#include "ReleaseSubHSM.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    ReleaseStart,
    AlignB,
    AlignF,
    FindTapeF,
    Release,
    ShimmyF,
    ShimmyB,
    Middle
} ReleaseSubHSMState_t;

static const char *StateNames[] = {
    "InitPSubState",
    "ReleaseStart",
    "AlignB",
    "AlignF",
    "FindTapeF",
    "Release"
    "ShimmyF",
    "ShimmyB",
    "Middle"
};

static uint8_t makeTransition = FALSE; // use to flag transition
static ReleaseSubHSMState_t nextState; // <- change type to correct enum



/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static ReleaseSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;


void checkDrop(ES_Event ThisEvent){
    if((ThisEvent.EventType == SIDE_CHANGE && ThisEvent.EventParam & SIDE_TAPE) || 
            (ThisEvent.EventType == TIME_TICK && SideQuery() > SIDE_UPPER_THRESHOLD)){
        MotorsSetSpeed(0, 0);
        nextState = Release;
        makeTransition = TRUE;
        ThisEvent.EventType = ES_NO_EVENT;
        ServoMove(RELEASE);
        ES_Timer_InitTimer(DEFAULT, 500);
    }
}


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitReleaseSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunReleaseSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunReleaseSubHSM(ES_Event ThisEvent) {
    ES_Tattle(); // trace call stack
    makeTransition = FALSE;
    static int TimedOut = FALSE;

    if (ThisEvent.EventType == ES_TOP_ENTRY) {
        CurrentState = ReleaseStart;
    }

    switch (CurrentState) {
        case InitPSubState:
            if (ThisEvent.EventType != ES_EXIT)// only respond to ES_Init
            {
                makeTransition = TRUE;
                nextState = ReleaseStart;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case ReleaseStart: // If current state is initial Psedudo State
            // this is where you would put any actions associated with the
            // transition from the initial pseudo-state into the actual
            // initial state
            if (ThisEvent.EventType == ES_TOP_ENTRY) {
                MotorsSetSpeed(0, 0);
                nextState = Release;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                ServoMove(RELEASE);
                ES_Timer_InitTimer(DEFAULT, 500);
            }

            break;
/*
        case AlignF:
            if (ThisEvent.EventType == BUMPER_CHANGE && ThisEvent.EventParam & (BUMPERFL_INPUT_PIN | BUMPERLF_INPUT_PIN)) {
                MotorsSetSpeed(-200,-1000); // was -100 and -500
                nextState = AlignB;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                ES_Timer_InitTimer(DEFAULT, 200);
            }
            break;
            
        case AlignB:
            if (!TimedOut && ThisEvent.EventType == BUMPER_CHANGE && ThisEvent.EventParam & (BUMPERBL_INPUT_PIN | BUMPERLB_INPUT_PIN)) {
                MotorsSetSpeed(0,500);
                ES_Timer_InitTimer(DEFAULT, 150);
                nextState = Middle;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else if(ThisEvent.EventType == BUMPER_CHANGE && ThisEvent.EventParam & (BUMPERBL_INPUT_PIN | BUMPERLB_INPUT_PIN)){
                MotorsSetSpeed(200,1000);
                nextState = AlignF;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT){
                TimedOut = TRUE;
            }
            break;
        
        case Middle:
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT){
                MotorsSetSpeed(400, 400);
                nextState = FindTapeF;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case FindTapeF:
            checkDrop(ThisEvent);
            break;
*/            
            
        //******************** BELOW HERE IS THE RELEASE ROUTINE *************************
            
        case Release:
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT) {
                MotorsSetSpeed(800, 600);
                nextState = ShimmyF;
                ES_Timer_InitTimer(DEFAULT, 200);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case ShimmyF:
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT) {
                MotorsSetSpeed(-600, -800);
                nextState = ShimmyB;
                ES_Timer_InitTimer(DEFAULT, 500);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        
        case ShimmyB:
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT) {
                MotorsSetSpeed(0, 0);
                ES_Event score;
                score.EventType = SCORE;
                PostHSM(score);
            }
            break;

        default: // all unhandled states fall into here
            printf("UNHANDLED STATE!!!");
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunReleaseSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunReleaseSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

