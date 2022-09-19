/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel Elkaim and Soja-Marie Morgens
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is another template file for the SubHSM's that is slightly differet, and
 * should be used for all of the subordinate state machines (flat or heirarchical)
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
#include "DepositSubHSM.h" //#include all sub state machines called
#include "EvadeSubHSM.h" 
#include "FindSubHSM.h" 
#include "ReleaseSubHSM.h"
#include "CircleSubHSM.h"
/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/
//Include any defines you need to do

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/


typedef enum {
    InitPState,
    Find,
    Deposit,
    Evade,
    Release,
    Circle,
    Escape,
    TurnAway,
    Realign1,
    Realign2
} TemplateHSMState_t;

static const char *StateNames[] = {
    "InitPState",
    "Find",
    "Deposit",
    "Evade",
    "Release",
    "Circle",
    "Escape",
    "TurnAway",
    "Realign1",
    "Realign2"
};


/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine
   Example: char RunAway(uint_8 seconds);*/
/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static TemplateHSMState_t CurrentState = InitPState; // <- change enum name to match ENUM
static uint8_t MyPriority;

static uint8_t Scored = 0;
extern int lapped;

extern int dir;

uint8_t HandleTape = TRUE;

static uint8_t DoNotTransition = FALSE;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitHSM(uint8_t Priority) {
    MyPriority = Priority;
    // put us into the Initial PseudoState
    CurrentState = InitPState;
    // post the initial transition event

    TapeInit();
    BumperInit();
    TrackInit();
    BeaconInit();
    ServoInit();
    MotorsInit();
    ES_Timer_Init();
    //LED_Init();
    //LED_AddBanks(LED_BANK1);

    //LED_SetBank(LED_BANK1, 0xF);
    //LED_SetBank(LED_BANK2, 0xF);

    IO_PortsSetPortInputs(PORTX, PIN4);
    IO_PortsSetPortInputs(PORTX, PIN11);

    ServoMove(LOCK);

    if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostHSM(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateHSM(ES_Event ThisEvent)
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
ES_Event RunHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    TemplateHSMState_t nextState; // <- change type to correct enum
    uint16_t side_val = 0;
    //printf("k");
    //ES_Tattle(); // trace call stack
    /*
    if(SideQuery() > SIDE_UPPER_THRESHOLD){
        LED_SetBank(LED_BANK2, 0xF);
    } else {
        LED_SetBank(LED_BANK2, 0x0);
    }*/

#ifdef DEBUG
    printf("%s->", StateNames[CurrentState]);
#endif



    switch (CurrentState) {
        case InitPState: // If current state is initial Pseudo State

            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state
                // Initialize all sub-state machines
                InitFindSubHSM();
                //InitEvadeSubHSM();
                InitDepositSubHSM();
                InitReleaseSubHSM();
                InitCircleSubHSM();
                // now put the machine into the actual initial state
                nextState = Find;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                
                //printf("%d \r\n",(IO_PortsReadPort(PORTX) & PIN11));

                if (!(IO_PortsReadPort(PORTX) & PIN4)) {
                    printf("SETTING DIR!!!!! \r\n");
                    ES_Event Dir;
                    Dir.EventType = DIR_CHANGE;
                    PostHSM(Dir);
                    //batteryConnect = TRUE;
                }
                
                if (!(IO_PortsReadPort(PORTX) & PIN11)) {
                    printf("CHANGING TAPE ROUTINE!!!!! \r\n");
                    HandleTape = FALSE;
                    //batteryConnect = TRUE;
                }
            }
            break;

        case Find: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            //LED_SetBank(LED_BANK1, 0x1);
            //if (batteryConnect) {
            ThisEvent = RunFindSubHSM(ThisEvent);
            if ((ThisEvent.EventType == BUMPER_CHANGE && (ThisEvent.EventParam & BUMPERFL_INPUT_PIN || ThisEvent.EventParam & BUMPERFR_INPUT_PIN)) ||
                    (ThisEvent.EventType == TIME_TICK && (BumperQuery() & BUMPERFL_INPUT_PIN || BumperQuery() & BUMPERFR_INPUT_PIN))) {
                nextState = Deposit;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                SetLapped(0);
            }
            //}
            break;

        case Deposit:
            //LED_SetBank(LED_BANK1, 0x2);

            //printf("In Deposit \n");

            ThisEvent = RunDepositSubHSM(ThisEvent);

            side_val = SideQuery();

            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DONOT) {
                DoNotTransition = FALSE;
            }


            if (ThisEvent.EventType == TIME_TICK && side_val > 420 && side_val < 840 && QueryFrontTrack() > UPPER_THRESHOLD && !Scored && getLapped()) {
                nextState = Release;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                ServoMove(RELEASE);
                MotorsSetSpeed(0, 0);
                SetLapped(0);
            } else if ((ThisEvent.EventType == BEACON_CHANGE && ThisEvent.EventParam == 1 && Scored) || (ThisEvent.EventType == TIME_TICK && getLapped() && GetNotSeenWhite() && BeaconQuery())) {
                nextState = TurnAway;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                MotorsSetSpeed(-250, -1000);
                ES_Timer_InitTimer(DEFAULT, 500);
                SetLapped(0);
            } else if (((ThisEvent.EventType == TAPE_CHANGE && ThisEvent.EventParam & B1_TAPE) || (ThisEvent.EventType == TIME_TICK && TapeQuery() & B1_TAPE)) && !DoNotTransition && HandleTape) {
                nextState = Circle;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                ES_Timer_InitTimer(DONOT, 1500);
                MotorsSetSpeed(0, 0);
                DoNotTransition = TRUE;
            }
            break;

        case Release:
            //LED_SetBank(LED_BANK1, 0x3);
            ThisEvent = RunReleaseSubHSM(ThisEvent);
            if (ThisEvent.EventType == SCORE) {
                nextState = Deposit;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                ServoMove(LOCK);
                Scored = 1;
            }
            break;

        case Escape:
            if ((ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT)) {
                nextState = Find;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                Scored = 0;
            }
            break;

        case TurnAway:
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT) {
                nextState = Escape;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                MotorsSetSpeed(1000, 600);
                ES_Timer_InitTimer(DEFAULT, 1200);
            }
            break;

        case Circle:
            RunCircleSubHSM(ThisEvent);

            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DONOT) {
                DoNotTransition = FALSE;
            }

            if (((ThisEvent.EventType == TAPE_CHANGE && ThisEvent.EventParam & B1_TAPE) || (ThisEvent.EventType == TIME_TICK && TapeQuery() & B1_TAPE)) && !DoNotTransition) {
                nextState = Realign1;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                MotorsSetSpeed(-1000, 1000);
                ES_Timer_InitTimer(DEFAULT, 250);
                SetLapped(2);
            } else if ((ThisEvent.EventType == BEACON_CHANGE && ThisEvent.EventParam == 1 && Scored) || (ThisEvent.EventType == TIME_TICK && GetNotSeenWhite() && BeaconQuery())) {
                nextState = TurnAway;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                MotorsSetSpeed(1000, -1000);
                ES_Timer_InitTimer(DEFAULT, 100);
            }
            break;

        case Realign1:
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT) {
                nextState = Realign2;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                MotorsSetSpeed(400, 1000);
                //ES_Timer_InitTimer(DEFAULT, 1000);

            }

        case Realign2:
            if ((ThisEvent.EventType == BUMPER_CHANGE && (ThisEvent.EventParam & BUMPERFL_INPUT_PIN || ThisEvent.EventParam & BUMPERFR_INPUT_PIN)) ||
                    (ThisEvent.EventType == TIME_TICK && (BumperQuery() & BUMPERFL_INPUT_PIN || BumperQuery() & BUMPERFR_INPUT_PIN))) {
                nextState = Deposit;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                DoNotTransition = TRUE;
                ES_Timer_InitTimer(DONOT, 1500);
            }

        default: // all unhandled states fall into here
            break;
    } // end switch on Current State
    //printf("t");


    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunHSM(TOP_ENTRY_EVENT); // <- rename to your own Run function
    }
    //printf("m");
    //ES_Tail(); // trace call stack end
    //printf("i");
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/
