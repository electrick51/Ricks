/**
 * UART interface used to download from the PC to the DataFlash.
 */
#include "twttsdef.h"
#include "uart2.h"
#include "dataflash.h"
#include <limits.h>

/** Macro used to calculate the number of elements of an array. */
#define NELEM(x) (sizeof(x)/sizeof(*x))

/**
 * The size of the data buffer makes room for the largest message: data read
 * or write. This message will contain the following bytes (maximum):
 *   2                message length
 *   1                command code
 *   1                ACK/NACK
 *   4                address
 *   FLASH_PAGE_SIZE  data bytes
 *   2                CRC
 *   6                dummy/error check
 */
struct uart
{
   unsigned char data[2 + 1 + 1 + 4 + FLASH_PAGE_SIZE + 2 + 6];
   uint16_t in, out;
};

/**
 * Our global variable: the input/output buffer.
 */
struct uart Uart;

/******************************************************************************/
struct dfl_state_machine
{
   
    enum dfl_state state;
   size_t bufsize;
};

struct dfl_state_machine DataFlashLoader =
{
   DFL_STATE_DISCONNECTED,
   0,
};
/******************************************************************************/

#if !defined FLASHPROG || FLASHPROG == 0
/**
 * UART receive interrupt handler: Read incoming byte from receive register,
 * place it into the receive buffer, and increment the incoming data counter
 *  (wrap the counter as necessary).
 */
void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
   Uart.data[Uart.in] = U2RXREG;
   if ( Uart.in < NELEM(Uart.data) - 1 )
   {
      ++Uart.in;
   }
   IFS1bits.U2RXIF = 0; /* clear interrupt flag */
}

/**
 * UART transmit interrupt handler: Write outgoing byte to the transmit
 * register and increment the incoming data counter (wrap the counter as
 * necessary).
 */
void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
   if ( Uart.out < Uart.in )
   {
      U2TXREG = Uart.data[Uart.out++];
   }
   else
   {
      Uart.in  = 0;
      Uart.out = 0;
      IEC1bits.U2TXIE = 0; /* Disable Transmit Interrupts */
      IEC1bits.U2RXIE = 1; /* Enable  Recieve  Interrupts */
   }
   IFS1bits.U2TXIF = 0; /* clear interrupt flag */
}
#endif

/**
 * Initialize UART2.
 */
void InitUART2(void)
{
/** Set up UART2 as interface to DataFlash. */
#if defined BoardRev && BoardRev == exp16
   RPINR19bits.U2RXR = 10;
   RPOR8bits.RP17R = U2TX_IO;
#else
   _TRISD1 = 1;      /* RDR1 is RX input */
   _U2RXR = 24;      /* RP24 = DEBUG_RX */
   _RP23R = U2TX_IO; /* RP23 = DEBUG_TX */
#endif
/** End of UART2 setup.  */

   // This is an EXAMPLE, so brutal typing goes into explaining all bit sets

   // The Explorer 16 board has a DB9 connector wired to UART2, so we will
   // be configuring this port only

   // configure U2MODE
   U2MODEbits.UARTEN = 0; // Bit15 TX, RX DISABLED, ENABLE at end of func
   U2MODEbits.USIDL  = 0; // Bit13 Continue in Idle
   U2MODEbits.IREN   = 0; // Bit12 No IR translation
   U2MODEbits.RTSMD  = 0; // Bit11 Simplex Mode
   U2MODEbits.UEN    = 0; // Bits8,9 TX,RX enabled, CTS,RTS not
   U2MODEbits.WAKE   = 0; // Bit7 No Wake up (since we don't sleep here)
   U2MODEbits.LPBACK = 0; // Bit6 No Loop Back
   U2MODEbits.ABAUD  = 0; // Bit5 No Autobaud (would require sending '55')
   U2MODEbits.RXINV  = 0; // Bit4 IdleState = 1
   U2MODEbits.BRGH   = 0; // Bit3 16 clocks per bit period
   U2MODEbits.PDSEL  = 0; // Bits1,2 8bit, No Parity
   U2MODEbits.STSEL  = 0; // Bit0 One Stop Bit

   U2BRG = 17; // baud rate 8=115200, 103=9600, 17=56k (55555)

   // Load all values in for U1STA SFR
   U2STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
   U2STAbits.UTXINV   = 0; //Bit14 N/A, IRDA config
   U2STAbits.UTXISEL0 = 0; //Bit13 Other half of Bit15
   U2STAbits.UTXBRK   = 0; //Bit11 Disabled
   U2STAbits.UTXEN    = 0; //Bit10 TX pins controlled by periph
   U2STAbits.UTXBF    = 0; //Bit9 *Read Only Bit*
   U2STAbits.TRMT     = 0; //Bit8 *Read Only bit*
   U2STAbits.URXISEL  = 0; //Bits6,7 Int. on character recieved
   U2STAbits.ADDEN    = 0; //Bit5 Address Detect Disabled
   U2STAbits.RIDLE    = 0; //Bit4 *Read Only Bit*
   U2STAbits.PERR     = 0; //Bit3 *Read Only Bit*
   U2STAbits.FERR     = 0; //Bit2 *Read Only Bit*
   U2STAbits.OERR     = 0; //Bit1 *Read Only Bit*
   U2STAbits.URXDA    = 0; //Bit0 *Read Only Bit*

   IFS1bits.U2TXIF    = 0; // Clear the Transmit Interrupt Flag
   IEC1bits.U2TXIE    = 0; // Disable Transmit Interrupts
   IFS1bits.U2RXIF    = 0; // Clear the Recieve Interrupt Flag
   IEC1bits.U2RXIE    = 1; // Enable Recieve Interrupts

   U2MODEbits.UARTEN = 1;  // And turn the peripheral on

   U2STAbits.UTXEN = 1;
}

/**
 * The CRC calculation used with Greybox; uses A001 polynomial.
 * @param  byte  pointer to the start of the checksum block
 * @param  size  the number of bytes in the checksum block
 * @return 16-bit CRC checksum
 */
uint16_t crc16(const uint8_t *byte, uint8_t size)
{
   uint16_t crc = 0;
   while ( size-- )
   {
      uint8_t bit = CHAR_BIT;
      unsigned char data = *byte++;
      while ( bit-- )
      {
         data ^= crc & 1;
         crc >>= 1;
         if ( data & 1 )
         {
            crc ^= 0xA001;
         }
         data >>= 1;
      }
   }
   return crc;
}

/**
 * Add message length to beginning and the CRC to the end of the message.
 * The contents of the message are presumed set already.
 * @param length  the number of bytes /not/ including the message length and CRC
 */
void uart_tx_send(uint16_t length)
{
   uint16_t  crc;
   /**
    * Add the message length to the beginning.
    */
   Uart.data[0] = length;      /* lsb */
   Uart.data[1] = length >> 8; /* msb */
   /**
    * Add the CRC to the end.
    */
   crc  = crc16(Uart.data, length - 2);
   Uart.data[length - 2] = crc;      /* lsb */
   Uart.data[length - 1] = crc >> 8; /* msb */
   /**
    * Set "bytes to transmit" index to the total number of bytes to transmit.
    */
   Uart.in = length; /** total bytes to transmit */
   /**
    * Set up the outgoing index and begin transmission.
    */
   Uart.out = 1; /* set index to the next byte to send (handled in ISR) */
   U2TXREG = Uart.data[0]; /* start transmission */
   IEC1bits.U2TXIE = 1;    /* Enable Transmit Interrupts */
}

/**
 * Send a response to a command.
 * 
 * <pre>
 * Bytes  Description
 * 0 - 1   size of this message (16-bit, little endian)
 *   2     command code
 *   3     ACK/NAK/ENQ
 * 4 - 5   crc (16-bit, little endian
 * </pre>
 * 
 * @return total number bytes to transmit (5)
 */
void uart_tx_ack(void)
{
   Uart.data[3] = DFL_ACK;
   uart_tx_send(6);
}

/**
 * Send a response to a command.
 * 
 * <pre>
 * Bytes  Description
 * 0 - 1   size of this message (16-bit, little endian)
 *   2     command code
 *   3     ACK/NAK/ENQ
 * 4 - 5   crc (16-bit, little endian
 * </pre>
 */
void uart_tx_nack(void)
{
   Uart.data[3] = DFL_NAK;
   uart_tx_send(6);
}

/**
 * Send a response to the DFL_PAGESIZE command.
 * 
 * <pre>
 * Bytes  Description
 * 0 - 1   size of this message (16-bit, little endian)
 *   2     command code
 *   3     ACK/NAK/ENQ
 * 4 - 5   flash page size (16-bit, little endian)
 * 6 - 7   crc (16-bit, little endian
 * </pre>
 */
void uart_tx_pagesize(void)
{
   /**
    * Command code.
    */
   Uart.data[2] = DFL_PAGESIZE;
   Uart.data[3] = DFL_ACK;
   /**
    * Add size data to prepopulated message.
    */
   Uart.data[4] = (uint8_t)FLASH_PAGE_SIZE;      /* lsb */
   Uart.data[5] = FLASH_PAGE_SIZE >> 8; /* msb */

   uart_tx_send(8);
}

/**
 * 
 * @param msg
 * 
 * @return 0 if received data is a valid message (CRC verified), 1 otherwise
 */
uint8_t uart_rx_valid(void)
{
   const uint16_t *length = (const uint16_t *)Uart.data;
   if ( *length < 4 )
   {
      return 0; /* Too short to do much of anything with at this point. */
   }
   if ( Uart.in - Uart.out < *length )
   {
      return 0; /* Received less than we are expecting. */
   }
   return crc16(Uart.data, *length) == 0;
}

/**
 * Check for incoming messages and respond to them.
 */
void dfloader_thread(void)
{
   if ( uart_rx_valid() )
   {
      IEC1bits.U2RXIE = 0; /* Disable Recieve  Interrupts */
      switch ( Uart.data[2] )
      {
      case DFL_CONNECT:
         uart_tx_ack();
         break;

      case DFL_DISCONNECT:
         uart_tx_ack();
         /* kill thread? */
         break;

      case DFL_PAGESIZE:
         uart_tx_pagesize();
         break;

      case DFL_WRITE:
         {
            /** Total message length. */
            uint16_t length  = ((uint16_t)Uart.data[0] <<  0)
                               | ((uint16_t)Uart.data[1] <<  8);
            /** Starting address for incoming data. */
            uint32_t address = ((uint32_t)Uart.data[5] <<  0)
                               | ((uint32_t)Uart.data[6] <<  8)
                               | ((uint32_t)Uart.data[7] << 16)
                               | ((uint32_t)Uart.data[8] << 24);
            /**
             * flash_rewrite() with received data
             * 
             * The length to write to flash is derived from the total message
             * length, subtracting off 9 bytes:
             *    2 for the length itself
             *    1 for the command code
             *    4 for the address
             *    2 for the CRC
             */
            flash_rewrite(address, (char *)&Uart.data[9], length - 9);
         }
         uart_tx_ack();
         break;

      default:
         uart_tx_nack();
         break;
      }
   }
}


