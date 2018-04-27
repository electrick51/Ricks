/**
  UART3 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    uart3.c

  @Summary
    This is the generated source file for the UART3 driver using Foundation Services Library

  @Description
    This source file provides APIs for driver for UART3. 
    Generation Information : 
        Product Revision  :  Foundation Services Library - pic24-dspic-pic32mm : v1.26
        Device            :  PIC24FJ1024GB610
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.30
        MPLAB 	          :  MPLAB X 3.45
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include "uart3.h"

/**
  Section: Data Type Definitions
*/

/** UART Driver Queue Status

  @Summary
    Defines the object required for the status of the queue.
*/

typedef union
{
    struct
    {
            uint8_t full:1;
            uint8_t empty:1;
            uint8_t reserved:6;
    }s;
    uint8_t status;
}

UART_BYTEQ_STATUS;

/** UART Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

*/
typedef struct
{
    /* RX Byte Q */
    uint8_t                                      *rxTail ;

    uint8_t                                      *rxHead ;

    /* TX Byte Q */
    uint8_t                                      *txTail ;

    uint8_t                                      *txHead ;

    UART_BYTEQ_STATUS                        rxStatus ;

    UART_BYTEQ_STATUS                        txStatus ;

} UART_OBJECT ;

static UART_OBJECT uart3_obj ;

/** UART Driver Queue Length

  @Summary
    Defines the length of the Transmit and Receive Buffers

*/

#define UART3_CONFIG_TX_BYTEQ_LENGTH 8
#define UART3_CONFIG_RX_BYTEQ_LENGTH 8


/** UART Driver Queue

  @Summary
    Defines the Transmit and Receive Buffers

*/

static uint8_t uart3_txByteQ[UART3_CONFIG_TX_BYTEQ_LENGTH] ;
static uint8_t uart3_rxByteQ[UART3_CONFIG_RX_BYTEQ_LENGTH] ;

void (*UART3_TxDefaultInterruptHandler)(void);
void (*UART3_RxDefaultInterruptHandler)(void);

/**
  Section: Driver Interface
*/


void UART3_Initialize (void)
{
   // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; URXINV disabled; UEN TX_RX; 
   U3MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit   
   // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; URXEN disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
   U3STA = 0x0000;
   // BaudRate = 115200; Frequency = 8000000 Hz; U3BRG 16; 
   U3BRG = 0x0010;
   // ADMADDR 0; ADMMASK 0; 
   U3ADMD = 0x0000;
    
   IEC5bits.U3RXIE = 1;
   UART3_SetRxInterruptHandler(UART3_Receive_ISR);
   UART3_SetTxInterruptHandler(UART3_Transmit_ISR);

    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
   U3MODEbits.UARTEN = 1;  // enabling UART ON bit
   U3STAbits.UTXEN = 1;
   
   

   uart3_obj.txHead = uart3_txByteQ;
   uart3_obj.txTail = uart3_txByteQ;
   uart3_obj.rxHead = uart3_rxByteQ;
   uart3_obj.rxTail = uart3_rxByteQ;
   uart3_obj.rxStatus.s.empty = true;
   uart3_obj.txStatus.s.empty = true;
   uart3_obj.txStatus.s.full = false;
   uart3_obj.rxStatus.s.full = false;
}




/**
    Maintains the driver's transmitter state machine and implements its ISR
*/
void UART3_SetTxInterruptHandler(void* handler){
    UART3_TxDefaultInterruptHandler = handler;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U3TXInterrupt ( void )
{
    (*UART3_TxDefaultInterruptHandler)();
}

void UART3_Transmit_ISR(void)
{ 
    if(uart3_obj.txStatus.s.empty)
    {
        IEC5bits.U3TXIE = false;
        return;
    }

    IFS5bits.U3TXIF = false;

    while(!(U3STAbits.UTXBF == 1))
    {

        U3TXREG = *uart3_obj.txHead;

        uart3_obj.txHead++;

        if(uart3_obj.txHead == (uart3_txByteQ + UART3_CONFIG_TX_BYTEQ_LENGTH))
        {
            uart3_obj.txHead = uart3_txByteQ;
        }

        uart3_obj.txStatus.s.full = false;

        if(uart3_obj.txHead == uart3_obj.txTail)
        {
            uart3_obj.txStatus.s.empty = true;
            break;
        }
    }
}

void UART3_SetRxInterruptHandler(void* handler){
    UART3_RxDefaultInterruptHandler = handler;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U3RXInterrupt( void )
{
    (*UART3_RxDefaultInterruptHandler)();
}

void UART3_Receive_ISR(void)
{


    while((U3STAbits.URXDA == 1))
    {

        *uart3_obj.rxTail = U3RXREG;

        uart3_obj.rxTail++;

        if(uart3_obj.rxTail == (uart3_rxByteQ + UART3_CONFIG_RX_BYTEQ_LENGTH))
        {
            uart3_obj.rxTail = uart3_rxByteQ;
        }

        uart3_obj.rxStatus.s.empty = false;
        
        if(uart3_obj.rxTail == uart3_obj.rxHead)
        {
            //Sets the flag RX full
            uart3_obj.rxStatus.s.full = true;
            break;
        }
        
    }

    IFS5bits.U3RXIF = false;
   
}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _U3ErrInterrupt ( void )
{
    if ((U3STAbits.OERR == 1))
    {
        U3STAbits.OERR = 0;
    }

    IFS5bits.U3ERIF = false;
}

/**
  Section: UART Driver Client Routines
*/

uint8_t UART3_Read( void)
{
    uint8_t data = 0;

    data = *uart3_obj.rxHead;

    uart3_obj.rxHead++;

    if (uart3_obj.rxHead == (uart3_rxByteQ + UART3_CONFIG_RX_BYTEQ_LENGTH))
    {
        uart3_obj.rxHead = uart3_rxByteQ;
    }

    if (uart3_obj.rxHead == uart3_obj.rxTail)
    {
        uart3_obj.rxStatus.s.empty = true;
    }

    uart3_obj.rxStatus.s.full = false;

    return data;
}


unsigned int UART3_ReadBuffer( uint8_t *buffer, unsigned int bufLen)
{
    unsigned int numBytesRead = 0 ;
    while ( numBytesRead < ( bufLen ))
    {
        if( uart3_obj.rxStatus.s.empty)
        {
            break;
        }
        else
        {
            buffer[numBytesRead++] = UART3_Read () ;
        }
    }

    return numBytesRead ;
}



void UART3_Write( uint8_t byte)
{
    IEC5bits.U3TXIE = false;
    
    *uart3_obj.txTail = byte;

    uart3_obj.txTail++;
    
    if (uart3_obj.txTail == (uart3_txByteQ + UART3_CONFIG_TX_BYTEQ_LENGTH))
    {
        uart3_obj.txTail = uart3_txByteQ;
    }

    uart3_obj.txStatus.s.empty = false;

    if (uart3_obj.txHead == uart3_obj.txTail)
    {
        uart3_obj.txStatus.s.full = true;
    }

    IEC5bits.U3TXIE = true ;
	
}


unsigned int UART3_WriteBuffer( uint8_t *buffer , unsigned int bufLen )
{
    unsigned int numBytesWritten = 0 ;

    while ( numBytesWritten < ( bufLen ))
    {
        if((uart3_obj.txStatus.s.full))
        {
            break;
        }
        else
        {
            UART3_Write (buffer[numBytesWritten++] ) ;
        }
    }

    return numBytesWritten ;

}


UART3_TRANSFER_STATUS UART3_TransferStatusGet (void )
{
    UART3_TRANSFER_STATUS status = 0;

    if(uart3_obj.txStatus.s.full)
    {
        status |= UART3_TRANSFER_STATUS_TX_FULL;
    }

    if(uart3_obj.txStatus.s.empty)
    {
        status |= UART3_TRANSFER_STATUS_TX_EMPTY;
    }

    if(uart3_obj.rxStatus.s.full)
    {
        status |= UART3_TRANSFER_STATUS_RX_FULL;
    }

    if(uart3_obj.rxStatus.s.empty)
    {
        status |= UART3_TRANSFER_STATUS_RX_EMPTY;
    }
    else
    {
        status |= UART3_TRANSFER_STATUS_RX_DATA_PRESENT;
    }
    return status;
}


uint8_t UART3_Peek(uint16_t offset)
{
    if( (uart3_obj.rxHead + offset) > (uart3_rxByteQ + UART3_CONFIG_RX_BYTEQ_LENGTH))
    {
      return uart3_rxByteQ[offset - (uart3_rxByteQ + UART3_CONFIG_RX_BYTEQ_LENGTH - uart3_obj.rxHead)];
    }
    else
    {
      return *(uart3_obj.rxHead + offset);
    }
}


uint8_t UART3_is_rx_ready(void)
{
    if(!uart3_obj.rxStatus.s.full)
    {
        if(uart3_obj.rxHead > uart3_obj.rxTail)
        {
            return(uart3_obj.rxHead - uart3_obj.rxTail);
        }
        else
        {
            return(UART3_CONFIG_RX_BYTEQ_LENGTH - (uart3_obj.rxTail - uart3_obj.rxHead));
        } 
    }
    return 0;
}


uint8_t UART3_is_tx_ready(void)
{
    if(!uart3_obj.txStatus.s.full)
    { 
        if(uart3_obj.txHead > uart3_obj.txTail)
        {
            return(uart3_obj.txHead - uart3_obj.txTail);
        }
        else
        {
            return(UART3_CONFIG_TX_BYTEQ_LENGTH - (uart3_obj.txTail - uart3_obj.txHead));
        }
    }
    return 0;
}


bool UART3_ReceiveBufferIsEmpty (void)
{
    return(uart3_obj.rxStatus.s.empty);
}


bool UART3_TransmitBufferIsFull(void)
{
    return(uart3_obj.txStatus.s.full);
}


UART3_STATUS UART3_StatusGet (void)
{
    return U3STA;
}

bool UART3_is_tx_done(void)
{
    return U3STAbits.TRMT;
}


/**
  End of File
*/
