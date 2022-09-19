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
#include "DepositSubHSM.h"
#include "motor.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    DepositStart,
    WallHug,
    Realign,
            MakeTurn,
            Aligned,
            R1
} DepositSubHSMState_t;

static const char *StateNames[] = {
    "InitPSubState",
    "DepositStart",
    "WallHug",
    "Realign",
    "MakeTurn",
    "Aligned",
    "R1"
};



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

static DepositSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;

static uint8_t makeTransition = FALSE; // use to flag transition
static DepositSubHSMState_t nextState; // <- change type to correct enum

static int lapped = 0;
static int NotSeenWhite = TRUE;

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
uint8_t InitDepositSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunDepositSubHSM(INIT_EVENT);
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
ES_Event RunDepositSubHSM(ES_Event ThisEvent) {
    makeTransition = FALSE;
    ES_Tattle(); // trace call stack
    
#ifdef DEBUG
    //if(ThisEvent.EventType != ES_ENTRY && ThisEvent.EventType != ES_EXIT){
    if(ThisEvent.EventType == TAPE_CHANGE){
        printf("%s | Event = %s \r\n",StateNames[CurrentState], EventNames[ThisEvent.EventType]);
    }
#endif
    
    
    if(ThisEvent.EventType == ES_TOP_ENTRY){
        CurrentState = DepositStart;
        NotSeenWhite = TRUE;
    }
    
    if(SideQuery() < 45){
        NotSeenWhite = FALSE;
        //printf("Seen white \n\r");
        //LED_SetBank(LED_BANK1, 0x0);
    }

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                nextState = DepositStart;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case DepositStart: // in the first state, replace this with correct names
            //printf("depositstart \n\r");
            if (ThisEvent.EventType == ES_TOP_ENTRY) {
                MotorsSetSpeed(400, -400);
                nextState = R1;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                ES_Timer_InitTimer(REALIGN, 3000);
                //lapped = 0;
            } else if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == REALIGN){
                MotorsSetSpeed(400, -400);
                nextState = R1;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                ES_Timer_InitTimer(REALIGN, 3000);
            }
            
            break;


        case WallHug: // in the first state, replace this with correct names
            if (((ThisEvent.EventType == BUMPER_CHANGE && ThisEvent.EventParam != 0))
                    && !(ThisEvent.EventParam & BUMPERLB_INPUT_PIN || ThisEvent.EventParam & BUMPERBL_INPUT_PIN)) {
                MotorsSetSpeed(500, -500);
                nextState = Realign;
                ES_Timer_InitTimer(DEFAULT, 500);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } /*else if(ThisEvent.EventType == BUMPER_CHANGE && ThisEvent.EventParam & BUMPERLF_INPUT_PIN){
                MotorsSetSpeed(1000, 1000);
                nextState = Aligned;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }*/
            
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT && (!(BumperQuery() & (BUMPERLF_INPUT_PIN | BUMPERFL_INPUT_PIN)))) {
                MotorsSetSpeed(-80, 1000);
                nextState = MakeTurn;
                ES_Timer_InitTimer(DEFAULT, 1700);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            } else if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT){
                ES_Timer_InitTimer(REALIGN, 200);
            }
                
            if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == REALIGN){
                MotorsSetSpeed(500, -500);
                nextState = Realign;
                ES_Timer_InitTimer(DEFAULT, 500);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;


            
        case Realign:

            if (ThisEvent.EventType == ES_ENTRY){
                lapped++;
            }
            if ((ThisEvent.EventType == BUMPER_CHANGE && (ThisEvent.EventParam & BUMPERLB_INPUT_PIN || ThisEvent.EventParam & BUMPERBL_INPUT_PIN)) ||
                    (ThisEvent.EventType == TIME_TICK && (BumperQuery() & BUMPERLB_INPUT_PIN || BumperQuery() & BUMPERBL_INPUT_PIN))) {
                MotorsSetSpeed(300, 600);
                nextState = WallHug;
                ES_Timer_InitTimer(DEFAULT, 450);
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT) {
                MotorsSetSpeed(300, 600);
                nextState = WallHug;
                makeTransition = TRUE;
                ES_Timer_InitTimer(DEFAULT, 450);
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

            
        case MakeTurn:
            if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == DEFAULT) {
                MotorsSetSpeed(300, 600);
                nextState = WallHug;
                makeTransition = TRUE;
                ES_Timer_InitTimer(DEFAULT, 450);
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == BUMPER_CHANGE && ThisEvent.EventParam != 0 
                    || (ThisEvent.EventType == TIME_TICK && BumperQuery())) {
                MotorsSetSpeed(500, -500);
                ES_Timer_InitTimer(DEFAULT, 500);
                nextState = Realign;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        /*
        case Aligned:
            if (ThisEvent.EventType == BUMPER_CHANGE && !(ThisEvent.EventParam & BUMPERLF_INPUT_PIN) ||) {
                MotorsSetSpeed(550, 1000);
                nextState = WallHug;
                makeTransition = TRUE;
                ES_Timer_InitTimer(DEFAULT, 400);
                ThisEvent.EventType = ES_NO_EVENT;
            } else if (ThisEvent.EventType == BUMPER_CHANGE && !(ThisEvent.EventParam & BUMPERLB_INPUT_PIN || ThisEvent.EventParam & BUMPERBL_INPUT_PIN)){
                MotorsSetSpeed(400, -400);
                ES_Timer_InitTimer(DEFAULT, 500);
                nextState = Realign;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
*/
        case R1:
            if ((ThisEvent.EventType == BUMPER_CHANGE && (ThisEvent.EventParam & BUMPERLB_INPUT_PIN || ThisEvent.EventParam & BUMPERBL_INPUT_PIN))
                    || (ThisEvent.EventType == TIME_TICK && (BumperQuery() & BUMPERLB_INPUT_PIN || BumperQuery() & BUMPERBL_INPUT_PIN))) {
                MotorsSetSpeed(0, 1000);
                nextState = WallHug;
                makeTransition = TRUE;
                ES_Timer_InitTimer(DEFAULT, 2500);
                ThisEvent.EventType = ES_NO_EVENT;
            } else if((ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == REALIGN)){
                MotorsSetSpeed(-600, -1000);
                nextState = DepositStart;
                makeTransition = TRUE;
                ES_Timer_InitTimer(REALIGN, 70);
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        default: // all unhandled states fall into here
            printf("ENTERED UNKNOWN STATE!!!! \n \r");
            break;
    } // end switch on Current State

    
    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunDepositSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunDepositSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}

// THIS WILL CAUSE PROBLEMS
int getLapped(void){ // NOTE THIS IS AWFUL CODE AND HIGHLY DEPENDENT ON THE TIMER TICK RATE 
    //printf("Lapped %d \n\r",lapped);
    if(lapped > 2){
        return 1;
    }
    return 0;
}

void SetLapped(int x){
    lapped = x;
}

int GetNotSeenWhite(void){
    return NotSeenWhite;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

