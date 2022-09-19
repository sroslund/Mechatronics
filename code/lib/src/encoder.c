#include "encoder.h"

#ifdef TEST_ENCODER
#define TEST_ENCODER_TIMER_PORT PORTY
#define TEST_ENCODER_TIMER_PIN PIN8
#endif /* TEST_ENCODER */

/**
 * Conversion macros from counts to degrees
 * 360 deg / 1300 counts
 * 1300 counts / 360 deg
 */
#define CONV_COUNTS_DEG 0.27692307
#define CONV_DEG_COUNTS 3.61111111

/** Free running us timer value */
static volatile unsigned int us;

typedef struct {
    /** Pole A */
    int A : 1;
    /** Pole B */
    int B : 1;
} Poles_t;

typedef struct {
    /** Previous state of poles */
    Poles_t poles;
    /** Current encoder count */
    int count;
    /** Time of last rising edge */
    unsigned int time;
    /** Period of encoder */
    unsigned int period;
} Status_t;

typedef struct {
    /** Left encoder */
    Status_t left;
    /** Right encoder */
    Status_t right;
} Encoders_t;

/** Module level variable for previous status of encoders */
static Encoders_t Encoders;

/**
 * @brief Update the status of a encoder
 * 
 * Reads the current state of poles A, B. If the state indicate movement
 * then counts will be updated. The period of the signal is updated on the
 * rising edge of the A pole.
 * 
 * @param _enc Pointer to encoder
 * @param _A_port Pole A port
 * @param _A_pin Pole A pin
 * @param _B_port Pole B port
 * @param _B_pin Pole B pin
 */
void UpdateStatus(Status_t * _enc, uint8_t _A_port, uint8_t _A_pin, uint8_t _B_port, uint8_t _B_pin);

char EncoderInit(void) {
    // Timer

    // Turn off and reset Timer3
    T3CON = 0;

    // Setup interrupt to trigger for 100us
    T3CONbits.TCKPS = 0b010;
    TMR3 = 0;
    PR3 = 1000;

    // Clear interrupt flag
    IFS0bits.T3IF = 0;
    // Set priority
    IPC3bits.T3IP = 3;
    IPC3bits.T3IS = 0;
    // Force interrupt enable
    IEC0bits.T3IE = 1;

    // Turn on timer
    T3CONbits.ON = 1;

    // Setup encoder pins as inputs
    if (IO_PortsSetPortInputs(ENCODER_LEFT_A_PORT, ENCODER_LEFT_A_PIN) == ERROR) {
        return ERROR;
    }
    if (IO_PortsSetPortInputs(ENCODER_LEFT_B_PORT, ENCODER_LEFT_B_PIN) == ERROR) {
        return ERROR;
    }
    if (IO_PortsSetPortInputs(ENCODER_RIGHT_A_PORT, ENCODER_RIGHT_A_PIN) == ERROR) {
        return ERROR;
    }
    if (IO_PortsSetPortInputs(ENCODER_RIGHT_B_PORT, ENCODER_RIGHT_B_PIN) == ERROR) {
        return ERROR;
    }

#ifdef TEST_ENCODER
    IO_PortsSetPortOutputs(TEST_ENCODER_TIMER_PORT, TEST_ENCODER_TIMER_PIN);
#endif /* TEST_ENCODER */

    return SUCCESS;
}

void EncoderResetCounts(void) {
    Encoders.left.count = 0;
    Encoders.right.count = 0;
}

int EncoderGetLeftCounts(void) {
    return Encoders.left.count;
}

int EncoderGetRightCounts(void) {
    return Encoders.right.count;
}

double EncoderGetLeftDegrees(void) {
    return EncoderGetLeftCounts() / CONV_DEG_COUNTS;
}

double EncoderGetRightDegrees(void) {
    return EncoderGetLeftCounts() / CONV_DEG_COUNTS;
}

unsigned int EncoderGetLeftPeriod(void) {
    return Encoders.left.period;
}

unsigned int EncoderGetRightPeriod(void) {
    return Encoders.right.period;
}

void UpdateStatus(Status_t * _enc, uint8_t _A_port, uint8_t _A_pin, uint8_t _B_port, uint8_t _B_pin) {
    // Read IO
    Poles_t curr;
    curr.A = !!(IO_PortsReadPort(_A_port) & _A_pin);
    curr.B = !!(IO_PortsReadPort(_B_port) & _B_pin);

    // Store reference to previous state
    Poles_t * prev = &(_enc->poles);

    if (!prev->A && !prev->B) {
        // Rising

        // Store previous encoder time
        unsigned int prev_time = _enc->time;

        if (curr.A && !curr.B) {
            // Forward
            // A = 1 and B = 0

            ++(_enc->count);

            // Update period
            _enc->time = us;
            _enc->period = _enc->time - prev_time;
        } else if (!curr.A && curr.B) {
            // Backward
            // A = 0 and B = 1

            --(_enc->count);

            // Update period
            _enc->time = us;
            _enc->period = _enc->time - prev_time;
        }
    } else if (prev->A && prev->B) {
        // Falling

        if (!curr.A && curr.B) {
            // Forward
            // A = 0 and B = 1

            ++(_enc->count);
        } else if (curr.A && !curr.B) {
            // Backward
            // A = 1 and B = 0

            --(_enc->count);
        }
    }

    // Update pole state
    _enc->poles = curr;
}

void __ISR(_TIMER_3_VECTOR) Timer3IntHandler(void) {
    // Increment frt
    us += 100;

#ifdef TEST_ENCODER
    IO_PortsTogglePortBits(TEST_ENCODER_TIMER_PORT, TEST_ENCODER_TIMER_PIN);
#endif /* TEST_ENCODER */

    UpdateStatus(&Encoders.left, ENCODER_LEFT_A_PORT, ENCODER_LEFT_A_PIN,
            ENCODER_LEFT_B_PORT, ENCODER_LEFT_B_PIN);
    UpdateStatus(&Encoders.right, ENCODER_RIGHT_A_PORT, ENCODER_RIGHT_A_PIN,
            ENCODER_RIGHT_B_PORT, ENCODER_RIGHT_B_PIN);

    // Clear flag
    IFS0bits.T3IF = 0;
}

#ifdef TEST_ENCODER
/* 
 * Runs the motor at a set rate and prints the period of the encoder. The timer
 * period can be verified by connecting pin defined by TEST_ENCODER_TIMER to a
 * oscope.
 */

#include <stdio.h>

#include "BOARD.h"
#include "timers.h"

int main(void) {
    BOARD_Init();
    printf("TEST_ENCODER, Compiled on %s, %s\r\n", __DATE__, __TIME__);
    TIMERS_Init();
    EncoderInit();

    unsigned int next_time;

    while (1) {
        printf("Period: %u\r\n", EncoderGetLeftPeriod());
        printf("Count: %u\r\n", EncoderGetLeftCounts());

        next_time = TIMERS_GetTime() + 1000;
        while (TIMERS_GetTime() < next_time);
    }

    while (1);

    return 0;
}

#endif /* TEST_ENCODER */