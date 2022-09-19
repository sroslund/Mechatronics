
/*
 * File:   uart.c
 * Author: mdunne
 *
 * Created on November 10, 2011, 8:42 AM
 */

#include <xc.h>
#include <serial.h>

#include <BOARD.h>
//#include <peripheral/uart.h>
#include <sys/attribs.h> //needed to use an interrupt
#include <stdint.h>
//#include <plib.h>
//#include <stdlib.h>



/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/

#define F_PB (BOARD_GetPBClock())
#define QUEUESIZE 512

/*******************************************************************************
 * PRIVATE DATATYPES                                                           *
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE FUNCTIONS PROTOTYPES                                                *
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES                                                           *
 ******************************************************************************/

static uint8_t AddingToTransmit = FALSE;
static uint8_t GettingFromReceive = FALSE;
static uint8_t TransmitCollisionOccured = FALSE;
static uint8_t ReceiveCollisionOccured = FALSE;

static uint8_t TransmitBuffer[QUEUESIZE];
static uint32_t TransmitHead = 0;
static uint32_t TransmitTail = 0;

static uint8_t ReceiveBuffer[QUEUESIZE];
static uint8_t ReceiveHead = 0;
static uint8_t ReceiveTail = 0;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/

/**
 * @Function SERIAL_Init(void)
 * @param none
 * @return none
 * @brief  Initializes the UART subsystem to 115200 and sets up the circular buffer
 * @author Max Dunne, 2011.11.10 */

void SERIAL_Init(void)
{
    U1MODE = 0; // Clear the control registers for the UART
    U1STA = 0;

    // calculate BRG value
    unsigned int brgValue = F_PB;
    brgValue >>= 3;
    brgValue /= 115200;
    brgValue++;
    brgValue >>= 1;
    brgValue--;
    U1BRG = brgValue; // set the baud rate to 115200
    // have interrupts occur whenever RX is not empty
    U1STAbits.URXISEL = 0;
    U1STAbits.UTXISEL = 0;

    IPC6bits.U1IP = 4; //set the interrupt priority

    // we now enable the module and both RX and TX
    U1MODEbits.ON = 1; // turn on uart itself
    U1STAbits.UTXEN = 1; // turn on TX
    U1STAbits.URXEN = 1; // turn on RX
    // enable the interrupts
    IEC0bits.U1RXIE = 1;
    IEC0bits.U1TXIE = 1;
}

/**
 * @Function PutChar(char ch)
 * @param ch - the char to be sent out the serial port
 * @return None.
 * @brief  adds char to the end of the circular buffer and forces the interrupt flag 
 * high if nothing is currently transmitting
 * @author Max Dunne, 2011.11.10 */
void PutChar(char ch)
{
    if (TransmitTail - TransmitHead != QUEUESIZE) {
        AddingToTransmit = TRUE;
        TransmitBuffer[TransmitTail] = ch;
        TransmitTail = (TransmitTail + 1) % QUEUESIZE;
        AddingToTransmit = FALSE;
        if (U1STAbits.TRMT) {
            IFS0bits.U1TXIF = 1;
        }
        //re-enter the interrupt if we removed a character while getting another one
        if (TransmitCollisionOccured) {
            IFS0bits.U1TXIF = 1;
            TransmitCollisionOccured = FALSE;

        }

    }
}

/**
 * @Function GetChar(void)
 * @param None.
 * @return ch - char from the serial port
 * @brief  reads first character from buffer or returns 0 if no chars available
 * @author Max Dunne, 2011.11.10 */
char GetChar(void)
{
    char ch;
    if (ReceiveTail == ReceiveHead) {
        ch = 0;
    } else {
        GettingFromReceive = TRUE;
        ch = ReceiveBuffer[ReceiveHead];
        ReceiveHead = (ReceiveHead+1) % QUEUESIZE;
        GettingFromReceive = FALSE;
    }
    //re-enter the interrupt if we added a character while transmitting another one
    if (ReceiveCollisionOccured) {
        IFS0bits.U1RXIF = 1;
        ReceiveCollisionOccured = FALSE;
    }
    return ch;
}

/**
 * @Function _mon_putc(char c)
 * @param c - char to be sent
 * @return None.
 * @brief  overwrites weakly define extern to use circular buffer instead of Microchip 
 * functions
 * @author Max Dunne, 2011.11.10 */
void _mon_putc(char c)
{
    PutChar(c);
}

/**
 * @Function _mon_puts(const char* s)
 * @param s - pointer to the string to be sent
 * @return None.
 * @brief  overwrites weakly defined extern to use circular buffer instead of Microchip 
 * functions
 * @author Max Dunne, 2011.11.10 */
void _mon_puts(const char* s)
{
    int i;
    for (i = 0; s[i] != '\0'; i++)
        PutChar(s[i]);
}

/**
 * @Function _mon_getc(int CanBlock)
 * @param CanBlock - unused variable but required to match Microchip form
 * @return None.
 * @brief  overwrites weakly defined extern to use circular buffer instead of Microchip 
 * functions
 * @author Max Dunne, 2011.11.10 */
int _mon_getc(int CanBlock)
{
    if (ReceiveTail == ReceiveHead)
        return -1;
    return GetChar();
}

/**
 * @Function IsReceiveEmpty(void)
 * @param None.
 * @return TRUE or FALSE
 * @brief  returns the state of the receive buffer
 * @author Max Dunne, 2011.12.15 */
char IsReceiveEmpty(void)
{
    if (ReceiveTail == ReceiveHead)
        return TRUE;
    return FALSE;
}

/**
 * @Function IsTransmitEmpty(void)
 * @param None.
 * @return TRUE or FALSE
 * @brief  returns the state of the receive buffer
 * @author Max Dunne, 2011.12.15 */
char IsTransmitEmpty(void)
{
    if (TransmitTail == TransmitHead)
        return TRUE;
    return FALSE;
}

/****************************************************************************
 Function
    IntUart1Handler

 Parameters
    None.

 Returns
    None.

 Description
    Interrupt Handle for the uart. with the PIC32 architecture both send and receive are handled within the same interrupt

 Notes


 Author
 Max Dunne, 2011.11.10
 ****************************************************************************/
void __ISR(_UART1_VECTOR) IntUart1Handler(void)
{
    if (IFS0bits.U1RXIF) {
        IFS0bits.U1RXIF = 0;
        if (!GettingFromReceive) {
            ReceiveBuffer[ReceiveTail] = U1RXREG;
            ReceiveTail = (ReceiveTail+1) % QUEUESIZE;
        } else {
            //acknowledge we have a collision and return
            ReceiveCollisionOccured = TRUE;
        }
    }

    if (IFS0bits.U1TXIF) {
        IFS0bits.U1TXIF = 0;
        if ((!AddingToTransmit)) {
            if (TransmitTail != TransmitHead) {
                U1TXREG = TransmitBuffer[TransmitHead];
                TransmitHead = (TransmitHead + 1) % QUEUESIZE;
            }
        } else {
            //acknowledge we have a collision and return
            TransmitCollisionOccured = TRUE;
        }

    }

}

/*******************************************************************************
 * PRIVATE FUNCTIONS                                                          *
 ******************************************************************************/


//#define SERIAL_TEST
#ifdef SERIAL_TEST
#include "serial.h"
#include "BOARD.h"
#include <GenericTypeDefs.h>
#include <stdio.h>

//#include <plib.h>
#define MAX_RAND (1<<10)
// inundation test should not be the default
//#define INUNDATION_TEST

int main(void)
{
    BOARD_Init();

    printf("\r\nUno Serial Test Harness\r\nAfter this Message the terminal should mirror any single character you type.%s\r\n", __TIME__);
    printf("\r\nContents of various configuration registers for reference below");
    printf("\r\nU1MODE: %X", U1MODE);
    printf("\r\nU1STA: %X", U1STA);
    printf("\r\nU1BRG: %d", U1BRG);
    printf("\r\nBRGH: %d", U1MODEbits.BRGH);

    printf("\r\n");
    while (!IsTransmitEmpty());

#ifdef INUNDATION_TEST
    unsigned int NopCount = 0;
    unsigned char CharCount = 0;
    unsigned int i;
    while (1) {
        NopCount = rand() % MAX_RAND + 1;
        //printf("%X\r\n",rand());
        for (i = 0; i < NopCount; i++) {
            asm("Nop");
        }
        for (CharCount = 32; CharCount < 128; CharCount++) {
            //printf("%c", CharCount);
            putchar(CharCount);
        }

    }
#endif
    while (1) {

//        if (IsTransmitEmpty() == TRUE) {
            if (IsReceiveEmpty() == FALSE) {
                PutChar(GetChar());

//            }
        }
    }
    while (1);
    return 0;
}

#endif