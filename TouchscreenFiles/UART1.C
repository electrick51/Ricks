#include "TWTTSDEF.H"
#include "uart.h"
//#include "uart1.h"
/// most of this file came from the Microchip software libraries
// for the PIC24F series chips

/* macros from uart.h
* listed here for convenience
#define EnableIntU1RX                   (IEC0bits.U1RXIE = 1)
#define EnableIntU1TX                   (IEC0bits.U1TXIE = 1)
#define DisableIntU1RX                   (IEC0bits.U1RXIE = 0)
#define DisableIntU1TX                  (IEC0bits.U1TXIE = 0)
#define SetPriorityIntU1RX(priority)     (IPC2bits.U1RXIP = priority)
#define SetPriorityIntU1TX(priority)     (IPC3bits.U1TXIP = priority)
#define U1RX_Clear_Intr_Status_Bit     (IFS0bits.U1RXIF = 0)
#define U1TX_Clear_Intr_Status_Bit     (IFS0bits.U1TXIF = 0)
*/

//#define BAUDRATE 50000
#define BAUDRATE 57600               // 57971@16MHz (+.6%)


/*************************************************************************
Function Prototype : uint8_t BusyUART1(void)

Include            : uart.h

Description        : This function returns the UART transmission status

Arguments          : None

Return Value       : If 1 is returned, it indicates that UART is busy in
                     transmission and UxSTA<TRMT> bit is 0.
                     If 0 is returned, it indicates that UART is not busy
                     and UxSTA<TRMT> bit is 1.

Remarks            : This function returns the status of the UART. This
                     indicates if the UART is busy in transmission as
                     indicated by the UxSTA<TRMT> bit.
*************************************************************************/
uint8_t BusyUART1( void )
{
    return (!U1STAbits.TRMT);
}

/*********************************************************************
Function Prototype : void CloseUART1(void)

Include            : uart.h

Description        : This function disables the UART and clears the
                     interrupt enable and flag bits.

Arguments          : None

Return Value       : None

Remarks            : This function first turns off the UART module and
                     then disables the UART transmit and receive interrupts.
                     The Interrupt Flag bits are also cleared.
*********************************************************************/
void CloseUART1( void )
{

    U1MODEbits.UARTEN = 0; // warning-uart pins revert to io pins

    IEC0bits.U1RXIE = 0;
    IEC0bits.U1TXIE = 0;

    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;
}

/**********************************************************************
Function Prototype : void ConfigIntUART1(uint16_t config)

Include            : uart.h

Description        : This function configures the UART Interrupts.

Arguments          : config - Individual interrupt enable/disable information
                     as defined below
                     Receive Interrupt enable
                        *UART_RX_INT_EN
                        *UART_RX_INT_DIS
                     Receive Interrupt Priority
                        *UART_RX_INT_PR0
                        *UART_RX_INT_PR1
                        *UART_RX_INT_PR2
                        *UART_RX_INT_PR3
                        *UART_RX_INT_PR4
                        *UART_RX_INT_PR5
                        *UART_RX_INT_PR6
                        *UART_RX_INT_PR7
                     Transmit Interrupt enable
                        *UART_TX_INT_EN
                        *UART_TX_INT_DIS
                     Transmit Interrupt Priority
                        *UART_TX_INT_PR0
                        *UART_TX_INT_PR1
                        *UART_TX_INT_PR2
                        *UART_TX_INT_PR3
                        *UART_TX_INT_PR4
                        *UART_TX_INT_PR5
                        *UART_TX_INT_PR6
                        *UART_TX_INT_PR7

Return Value       : None

Remarks            : This function enables/disables the UART transmit and
                     receive interrupts and sets the interrupt priorities.
**********************************************************************/

void ConfigIntUART1( uint16_t config )
{
    /* clear IF flags */
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;

    /* set priority */
    IPC2bits.U1RXIP = 0x0007 & config;
    IPC3bits.U1TXIP = (0x0070 & config) >> 4;

    /* enable/disable interrupt */
    IEC0bits.U1RXIE = (0x0008 & config) >> 3;
    IEC0bits.U1TXIE = (0x0080 & config) >> 7;
}

/*************************************************************************************
Function Prototype : uint8_t DataRdyUART1(void)

Include            : uart.h

Description        : This function returns the UART receive buffer status.

Arguments          : None

Return Value       : If 1 is returned, it indicates that the receive buffer
                     has a data to be read.
                     If 0 is returned, it indicates that receive buffer
                     does not have any new data to be read.

Remarks            : This function returns the status of the UART receive buffer.
                     This indicates if the UART receive buffer contains any new
                     data that is yet to be read as indicated by the UxSTA<URXDA> bit
**************************************************************************************/

uint8_t DataRdyUART1( void )
{
    return (U1STAbits.URXDA);
}

#if 0 //dormant
/***************************************************************************************
Function Prototype : uint16_t getsUART1(uint16_t length,uint16_t *buffer,
                                            uint16_t uart_data_wait)

Include            : uart.h

Description        : This function reads a string of data of specified length and
                     stores it into the buffer location specified.

Arguments          : length - This is the length of the string to be received.
                     buffer - This is the pointer to the location where the data
                     received  have to be stored.
                     uart_data_wait- This is the time-out count for which the module
                     has to wait before return. If the time-out count is ‘N’, the actual
                     time out would be about (19 * N – 1) instruction cycles.

Return Value       : This function returns the number of bytes yet to be received.
                     If the return value is ‘0’, it indicates that the complete string
                     has been received. If the return value is non-zero, it indicates that
                     the complete string has not been received

Remarks            : None
*******************************************************************************************/

uint16_t getsUART1( uint16_t length, uint16_t* buffer, uint16_t uart_data_wait )
{
    int16_t wait = 0;
    uint8_t* temp_ptr = (uint8_t *)buffer;

    while (length) /* read till length is 0 */
    {
        while (!DataRdyUART1())
        {
            if (wait < uart_data_wait)
                wait++;          /*wait for more data */
            else
                return (length); /*Time out- Return words/bytes to be read */
        }
        wait = 0;

        if (U1MODEbits.PDSEL == 3)        /* check if TX/RX is 8bits or 9bits */
            *buffer++ = U1RXREG;          /* data word from HW buffer to SW buffer */
        else
            *temp_ptr++ = U1RXREG & 0xFF; /* data byte from HW buffer to SW buffer */

        length--;
    }

    return (length); /* number of data yet to be received i.e.,0 */
}
#endif

/***************************************************************************************
Function Prototype : uint8_t getzUART1(uint8_t length,uint8_t *buffer,
                                            uint16_t uart_data_wait)

Include            : uart.h

Description        : This function reads a string of data of specified length and
                     stores it into the buffer location specified.

Arguments          : length - Length of the string to be received.
                     buffer - This is the pointer to the location where the data
                     received have to be stored.
                     uart_data_wait- This is the time-out count for which the module
                     has to wait before return. If the time-out count is N, the actual
                     time out would be about (19 * N - 1) instruction cycles.
                     N=(t*fpcu+1)/19000 where fcpu is in Hertz and t is msec

Return Value       : This function returns the number of bytes yet to be received.
                     If the return value is 0, it indicates that the complete string
                     has been received. If the return value is non-zero, it indicates that
                     the complete string has not been received

Remarks            : works in 8 bit mode only
                   : PIO, does not use interrupts
                   : 0 < buffer size must be <= 256
                   : uart rx buffer has a 4 deep fifo
*******************************************************************************************/

uint8_t getzUART1( uint8_t length, uint8_t* buffer, uint16_t uart_data_wait )
{
    int wait = 0;

    while (length) /* read till length is 0 */
    {
        while (!DataRdyUART1())
        {
            if (wait < uart_data_wait)
                wait++;          /*wait for more data */
            else
                return (length); /*Time out- Return words/bytes to be read */
        }

        wait = 0;
        *buffer++ = U1RXREG; /* data word from HW buffer to SW buffer */

        length--;
    }

    return (length); /* number of data yet to be received i.e.,0 */
}

/*******************************************************************************
Function Prototype : void OpenUART1(uint16_t config1,uint16_t config2,
                                    uint16_t ubrg)

Include            : uart.h

Description        : This function configures the UART module

Arguments          : config1 - This contains the parameters to be configured in the
                    UxMODE register as defined below
                    UART enable/disable
                        *UART_EN
                        *UART_DIS
                    UART Idle mode operation
                        *UART_IDLE_CON
                        *UART_IDLE_STOP
                    UART Wake-up on Start
                        *UART_EN_WAKE
                        *UART_DIS_WAKE
                    UART Loopback mode enable/disable
                        *UART_EN_LOOPBACK
                        *UART_DIS_LOOPBACK
                    Input to Capture module
                        *UART_EN_ABAUD
                        *UART_DIS_ABAUD
                    Parity and data bits select
                        *UART_NO_PAR_9BIT
                        *UART_ODD_PAR_8BIT
                        *UART_EVEN_PAR_8BIT
                        *UART_NO_PAR_8BIT
                    Number of Stop bits
                        *UART_2STOPBITS
                        *UART_1STOPBIT
                    IrDA encoder and decoder enable/disable
                        *UART_IrDA_ENABLE
                        *UART_IrDA_DISABLE
                    Mode Selection
                        *UART_MODE_SIMPLEX
                        *UART_MODE_FLOW
                    Enable bits
                        *UART_UEN_11
                        *UART_UEN_10
                        *UART_UEN_01
                        *UART_UEN_00
                    Receive Polarity Inversion bit
                        *UART_UXRX_IDLE_ZERO
                        *UART_UXRX_IDLE_ONE
                    High Baudrate Enable
                        *UART_BRGH_FOUR
                        *UART_BRGH_SIXTEEN


                    config2 - This contains the parameters to be configured in the
                    UxSTA register as defined below
                    UART Transmission mode interrupt select
                        *UART_INT_TX_BUF_EMPTY
                        *UART_INT_TX_LAST_CH
                        *UART_INT_TX_EACH_CHAR
                    UART IrDA polarity inversion bit
                        *UART_IrDA_POL_INV_ONE
                        *UART_IrDA_POL_INV_ZERO
                    UART Transmit Break bit
                        *UART_SYNC_BREAK_ENABLED
                        *UART_SYNC_BREAK_DISABLED
                    UART transmit enable/disable
                        *UART_TX_ENABLE
                        *UART_TX_DISABLE
                    UART Receive Interrupt mode select
                        *UART_INT_RX_BUF_FUL
                        *UART_INT_RX_3_4_FUL
                        *UART_INT_RX_CHAR
                    UART address detect enable/disable
                        *UART_ADR_DETECT_EN
                        *UART_ADR_DETECT_DIS
                    UART OVERRUN bit clear
                        *UART_RX_OVERRUN_CLEAR

                    ubrg - This is the value to be written into UxBRG register
                    to set the baud rate.

Return Value     :  None

Remarks          :  This functions configures the UART transmit and receive
                    sections and sets the communication baud rate
*********************************************************************/
void OpenUART1( uint16_t config1, uint16_t config2, uint16_t ubrg )
{
    U1BRG = ubrg;                /* baud rate */
    U1MODE = config1;            /* operation settings */
    U1STA = config2;             /* TX & RX interrupt modes */

    while (U1STAbits.URXDA == 1) /* Clear Buffer*/
    {
        uint8_t Temp;
        Temp = U1RXREG;
    }
    IFS0bits.U1RXIF = 0; /*clear interrupt flag*/
}

#if 1
/*********************************************************************************************
Function Prototype  : void putsUART1(uint16_t *buffer)

Include             : uart.h

Description         : This function writes a string of data to be transmitted
                      into the UART transmit buffer.

Arguments           : buffer - This is the pointer to the string of data to be transmitted.

Return Value        : None

Remarks             : This function writes the data to be transmitted into the transmit buffer
                      until NULL character is encountered. Once the transmit buffer is full,
                      it waits until data gets transmitted and then writes the next data into
                      the Transmit register.
************************************************************************************************/

void putsUART1( uint16_t* buffer )
{
    uint8_t* temp_ptr = (uint8_t *)buffer;

    /* transmit till NULL character is encountered */

    if (U1MODEbits.PDSEL == 3) /* check if TX is 8bits or 9bits */
    {
        while (*buffer != '\0')
        {
            while (U1STAbits.UTXBF); /* wait if the buffer is full */

            U1TXREG = *buffer++;     /* transfer data word to TX reg */
        }
    }
    else
    {
        while (*temp_ptr != '\0')
        {
            while (U1STAbits.UTXBF); /* wait if the buffer is full */
            U1TXREG = *temp_ptr++;   /* transfer data byte to TX reg */
        }
    }
}

#endif

#if 0
/************************************************************************
Function Prototype  : void putzUART1(uint16_t *buffer)

Include             : uart.h

Description         : This function writes a string of BYTES
                      into the UART transmit buffer.

Arguments           : buffer - Pointer to the string of bytes to be transmitted.
                               The first byte in the buffer is the number of bytes
                               to send minus one

Return Value        : None

Remarks             : This function writes the bytes to be transmitted
                      into the transmit buffer unit until N bytes are sent.
                      Once the transmit buffer is full, it waits until data
                      gets transmitted and then writes the next data into
                      the Transmit register.

Notes               : Since we are sending binary we cant use NULLs
                      to end the buffer.

                    : PIO, does not use interrupts
                    : 0 < buffer size must be <= 256
                    : uart tx buffer has a 4 deep fifo
************************************************************************************************/

void putzUART1( uint8_t* buffer )
{
    uint8_t* endptr;
    uint8_t val;
    val = *buffer;
    endptr = buffer + val; // point to last byte

    /* >>>> works for 8 bit data only <<<< */
    while (buffer <= endptr)
    {
        while (U1STAbits.UTXBF) { } /* wait if the buffer is full */
        U1TXREG = *buffer++;        /* transfer data byte to TX reg */
#if 1

        {
            uint16_t i, j;

            for (i = 0; i < 500; ++i)
            {
                for (j = 0; j < 1; ++j);
            }
        }

#endif

    }
}
#endif

/*********************************************************************************************
Function Prototype : uint16_t ReadUART1(void)

Include            : uart.h

Description        : This function returns the content of UART receive buffer
                     (UxRXREG) register.

Arguments          : None

Return Value       : This function returns the contents of Receive buffer
                     (UxRXREG) register.

Remarks            : This function returns the contents of the Receive Buffer register.
                     If 9 bit reception is enabled, the entire register content is returned.
                     If 8 bit reception is enabled, then register is read and the 9th bit is
                     masked.
**********************************************************************************************/

uint16_t ReadUART1( void )
{
    if (U1MODEbits.PDSEL == 3)
        return (U1RXREG);
    else
        return (U1RXREG & 0xFF);
}


/*********************************************************************
Function Prototype : void WriteUART1(uint16_t data)

Include            : uart.h

Description        : This function writes data to be transmitted into the
                     transmit buffer (UxTXREG) register.

Arguments          : data - This is the data to be transmitted.

Return Value       : None

Remarks            : This function writes the data to be transmitted into
                     the transmit buffer.
                     If 9-bit transmission is enabled, the 9-bit value is
                     written into the transmit buffer.
                     If 8-bit transmission is enabled, then upper byte is
                     masked and then written into the transmit buffer.
*********************************************************************/
void WriteUART1( uint16_t data )
{
    if (U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}

#undef _ISR
#define _ISR __attribute__((interrupt, auto_psv))

/* Transmit data is stored in array txdBuf  */

uint8_t txIndex;
#define SIZEOFTXBUF 256

uint8_t txdBuf[SIZEOFTXBUF];
/* Received data is stored in array rxdBuf  */
uint8_t rxIndex;
#define SIZEOFRXBUF 256

uint8_t rxdBuf[SIZEOFRXBUF];

#if 1

void _ISR _U1TXInterrupt( void )
{
    IFS0bits.U1TXIF = 0;       // reset tx interrupt request flag
    U1TXREG = txdBuf[txIndex]; // put byte in xmit reg

    if (txIndex < txdBuf[0])   // send n+1 bytes, n in txdBuf[0]
    {
        ++txIndex;
    }
    else
    {
        rxIndex = 0;

        while (U1STAbits.TRMT == 0) {
        // wait til transmit sr is empty
        }

        DisableIntU1TX;
        EnableIntU1RX;
    }
}

#endif

/* Received data is stored in array rxdBuf  */


/* This is UART1 receive ISR */

void _ISR _U1RXInterrupt( void )
{

    IFS0bits.U1RXIF = 0;                             // reset rx interrupt request flag

    while (U1STAbits.URXDA)                          // 4 byte fifo
    {
        rxdBuf[rxIndex] = (uint8_t)(U1RXREG & 0xFF); // 8 bit data only

        if (rxIndex < (SIZEOFRXBUF - 1))
        {
            ++rxIndex;
            continue;
        }
        else
        {
            DisableIntU1RX; // disable rx interrupt
            break;
        }
    }
}
#if 0

void xmitTestUART1( void )
{
    /* Data to be transmitted using UART communication module */
    uint8_t Txdata [] =
    {
        'M', 'i', 'c', 'r', 'o', 'c', 'h', 'i', 'p', 'I', 'C', 'D', '2', '\0'
    };

    //char Txdata[] = {'a','a','a','a','a','a','\0'};

    /* Load transmit buffer and transmit the same
       till null character is encountered */

    putzUART1((uint8_t *)Txdata);

    /* Wait for  transmission to complete */

    while (BusyUART1());
}

#endif

void rcvTestUART1( void )
{
    /* Read all the data remaining in receive buffer which are unread */

    while (DataRdyUART1()) {

    //        (*( Receiveddata)++) = ReadUART1() ;
    //        ( *( Receiveddata)) = ReadUART1();

    }
}

#if 0

void loopTestUART1( void )
{
    uint16_t i;

    uint8_t* Rcvddata = rxdBuf;

    for (i = 0; i < 80; ++i)
    {
        rxdBuf[i] = 0;
    }

    /* Data to be transmitted using UART communication module */
    uint8_t Txdata [] =
    {
        'M', 'i', 'c', 'r', 'o', 'c', 'h', 'i', 'p', 'I', 'C', 'D', '2', '\0'
    };


    /* Load transmit buffer and transmit the same
       till null character is encountered */

    putzUART1((uint8_t *)Txdata);

    /* Wait for  transmission to complete */

    while (BusyUART1());

    asm (" nop");
    asm (" nop");
    asm (" nop");

    return;

    /* Read all the data  */
    while (1)
    {
        if (DataRdyUART1())
        {

            (*(Rcvddata)) = ReadUART1();

            if ((*(Rcvddata)++) == 0)
            {
                break;
            }
        }
    ////    context_switch();
    }
}

#endif

void initUart1( void )
{

    /* Holds the value of baud register   */

    uint16_t baudvalue;

    /* Holds the value of uart config reg */

    uint16_t U1MODEvalue;

    /* Holds the information regarding uart

    TX & RX interrupt modes */

    uint16_t U1STAvalue;

    /* Turn off UART1module */

    CloseUART1();

    /* Configure uart1 receive and transmit interrupt */

    ConfigIntUART1(   \
    UART_RX_INT_EN &  \
    UART_RX_INT_PR6 & \
    UART_TX_INT_EN &  \
    UART_TX_INT_PR2);

    /* Configure UART1 module to transmit 8 bit data with one stopbit. */
    // baud rate = Fcpu/(4*(N+1)) for BRGH=1
    // N=Fcpu/(4*(baud rate))-1
    // baudvalue = 208 - 1;                 // 19231  for Fcpu=16MHz
    // baudvalue = 104 - 1;                 // 38461  for Fcpu=16MHz
    baudvalue=(FCPU/BAUDRATE/4)-1;
    U1MODEvalue = UART_EN &              \
    UART_IDLE_CON &                      \
    UART_DIS_WAKE &                      \
    UART_DIS_LOOPBACK &                  \
    UART_DIS_ABAUD &                     \
    UART_NO_PAR_8BIT &                   \
    UART_1STOPBIT &                      \
    UART_IrDA_DISABLE &                  \
    UART_MODE_SIMPLEX &                  \
    UART_UEN_00 &                        \
    UART_UXRX_IDLE_ONE &                 \
    UART_BRGH_FOUR;

    U1STAvalue = UART_INT_TX_BUF_EMPTY & \
    UART_IrDA_POL_INV_ZERO &             \
    UART_SYNC_BREAK_DISABLED &           \
    UART_TX_DISABLE &                    \
    UART_INT_RX_CHAR &                   \
    UART_ADR_DETECT_DIS &                \
    UART_RX_OVERRUN_CLEAR;

    OpenUART1(U1MODEvalue, U1STAvalue, baudvalue);
} // ends initUart1


/* Read all the data remaining in receive buffer which are unread */

//    while(DataRdyUART1())
//
//    {
//
//        (*( Receiveddata)++) = ReadUART1() ;
//
//    }

/* Turn off UART1 module */

//    CloseUART1();

//    return 0;

//
