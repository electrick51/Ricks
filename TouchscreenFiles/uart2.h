#ifndef UART2_H
#define UART2_H

/*****************************************************************************
 *
 * UART Driver for PIC24.
 *
 *****************************************************************************
 * FileName:        uart2.h
 * Dependencies:    
 * Processor:       PIC24
 * Compiler:       	MPLAB C30
 * Linker:          MPLAB LINK30
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company's
 * customer, for use solely and exclusively with products manufactured
 * by the Company. 
 *
 * The software is owned by the Company and/or its supplier, and is 
 * protected under applicable copyright laws. All rights are reserved. 
 * Any use in violation of the foregoing restrictions may subject the 
 * user to criminal sanctions under applicable laws, as well as to 
 * civil liability for the breach of the terms and conditions of this 
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, 
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT, 
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR 
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * A simple UART polled driver 
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Albert Z.			11/7/08		beta
 *****************************************************************************/
#include "p24fxxxx.h"
#include <stddef.h>

// External oscillator frequency
#define SYSCLK          8000000

// UART IOs
#if defined (__PIC24FJ64GA004__)
	#define UART2_TX_TRIS	TRISCbits.TRISC9
	#define UART2_RX_TRIS	TRISCbits.TRISC3
#else // PIC24FJ128GA010, PIC24FJ256GA110, PIC24FJ256GB110
	#define UART2_TX_TRIS   TRISFbits.TRISF5
	#define UART2_RX_TRIS   TRISFbits.TRISF4
#endif

//PPS Outputs
#ifndef __PIC24FJ128GA010__
	#define U2TX_IO		5
	#define U2RTS_IO	6
#endif

/*****************************************************************************
 * DEFINITIONS
 *****************************************************************************/
// Baudrate
#define BAUDRATE2		9600
/*****************************************************************************
 * U2BRG register value and baudrate mistake calculation
 *****************************************************************************/
#define BAUDRATEREG2 SYSCLK/32/BAUDRATE2-1

#if BAUDRATEREG2 > 255
#error Cannot set up UART2 for the SYSCLK and BAUDRATE.\
 Correct values in main.h and uart2.h files.
#endif

#define BAUDRATE_MISTAKE 1000*(BAUDRATE2-SYSCLK/32/(BAUDRATEREG2+1))/BAUDRATE2
#if (BAUDRATE_MISTAKE > 2)||(BAUDRATE_MISTAKE < -2)
#error UART2 baudrate mistake is too big  for the SYSCLK\
 and BAUDRATE2. Correct values in uart2.c file.
#endif 

/*****************************************************************************
 * Function: UART2Init
 *
 * Precondition: None.
 *
 * Overview: Setup UART2 module.
 *
 * Input: None.
 *
 * Output: None.
 *
 *****************************************************************************/
extern void InitUART2(void);

/*****************************************************************************
 * Function: UART2PutChar
 *
 * Precondition: UART2Init must be called before.
 *
 * Overview: Wait for free UART transmission buffer and send a byte.
 *
 * Input: Byte to be sent.
 *
 * Output: None.
 *
 *****************************************************************************/
extern void  UART2PutChar(char );

/*****************************************************************************
 * Function: UART2PutDec
 *
 * Precondition: UART2Init must be called before.
 *
 * Overview: This function converts decimal data into a string
 * and outputs it into UART.
 *
 * Input: Binary data.
 *
 * Output: None.
 *
 *****************************************************************************/
extern void  UART2PutDec(unsigned char );

void uart_send(const void *buffer, size_t size);

void dfloader_thread(void);

enum dfl_state
{
   DFL_STATE_DISCONNECTED,
   DFL_STATE_CONNECTED,
   DFL_STATE_RX_READY,
   DFL_STATE_TX_READY,
   DFL_STATE_TOTAL
};

/**
 * Values borrowed from ASCII.
 * http://www.asciitable.com/
 */
enum dfl_ack
{
   DFL_ENQ = 5,
   DFL_ACK = 6,
   DFL_NAK = 21
};

enum dfl_command
{
   DFL_DISCONNECT,
   DFL_CONNECT,
   DFL_PAGESIZE,
   DFL_WRITE,
   DFL_READ,
   DFL_TOTAL
};

#endif
