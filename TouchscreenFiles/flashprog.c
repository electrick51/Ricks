/**
 * This source code is used in a target device for loading an external DataFlash
 * with a binary file containing data such as string literals for display text,
 * voice token table, compressed voice data files, bitmaps, and other data.
 * 
 * The transfer method uses Hyperterminal from Windows and uses the XMODEM 1k
 * transfer method. This makes the file transmission handling on the PC end
 * fairly simple and straightforward -- using an existing application (Hyper-
 * terminal). This code is the source for the receiving end on the target
 * platform.
 * 
 * First created for SimonXT (or earlier), the code was tied to the specifics of
 * the Renesas part being used. Later this code was ported for use on the
 * Two-Way Talking Touch Screen which uses a Microchip part. An attempt was made
 * to preserve both versions using conditional compilation (NC30 is a predefined
 * macro for the Renesas toolchain).
 * 
 * Currently, only a binary file transfer, verify, and full chip erase are
 * supported as DataFlash commands.
 */
#include "dataflash.h"
#if defined NC30 /* Renesas part for SimonXT */
#include "sfr62p.h"
#else /* assumed Microchip part for TWTTS */
#include "twttsdef.h"
#include "uart2.h"
#endif

#include <stdio.h>
#include <string.h>

/**
 * ASCII codes used with XMODEM protocol.
 */
#define SOH_CODE  1
#define STX_CODE  2
#define NAK_CODE  21
#define EOT_CODE  4
#define ACK_CODE  6
#define GO_CODE   'C'
/**
 * Other XMODEM protocol-related definitions.
 */
#define MAX_ERROR  3
#define BIN_CHUNK 64  // must be integral part of 1024
#define HEX_CHUNK 32  // max hex record data size

#define FLASH_SIZE 0x1000000

#define StartOfFlash  0 //20*264 //12 * 264 // page =16, 264 = Flash pag size  = 16 * 264 = 0x1080

unsigned long START_OF_FLASH = StartOfFlash;

/**
 * Buffers and indexes. They are "global" in this module because they are
 * updated in interrupt handlers.
 */
// Uart buffers:
#define RX_CIRC_SIZE	4096	// big enough for two xmodem 1k blocks
#define BUFFER_FULL_THRESHHOLD 1500 //must be smaller than RX_CIRC_SIZE

static char rx_buffer[RX_CIRC_SIZE];
static volatile unsigned rx_in;
static volatile unsigned rx_out;
static volatile int xmodemPktSize; //1024 or 128

/**
 * Received interrupt function pointer: used to implement xmodem protocol state
 * machine.
 */
void (*volatile rx_func)(void);

//
// receive state functions for xmodem
//
static unsigned rx_in_xmodem;
static char xmodem_block_num;
static unsigned xmodem_error;
static unsigned xmodem_count;
#if !defined NC30
struct bit_def
{
   char  b0:1;
   char  b1:1;
   char  b2:1;
   char  b3:1;
   char  b4:1;
   char  b5:1;
   char  b6:1;
   char  b7:1;
};
union byte_def
{
   struct bit_def bit;
   char  byte;
};
#endif
static union byte_def _flags;
#define FLOW_CONTROL    _flags.bit.b0

/**
 * xmodem receive function prototypes
 */
static void rx_xmodem_stx(void);
static void rx_xmodem_blk(void);
static void rx_xmodem_nblk(void);
static void rx_xmodem_data(void);
static void rx_xmodem_crc1(void);
static void rx_xmodem_crc2(void);

/**
 * misc prototypes
 */
inline void update_crc(unsigned char c);
static void put_char(char c);
static void put_string(const char * pString);
static char get_char(void);
static char is_char(void);
static void flush_rx(void);
static void rx_terminal(void);
void uart_receive(void);     // receive ISR
void uart_transmit(void);    // transmit ISR (not used)
static void uart_setup(void);
static unsigned error_check(void);
static void xmodem_receive_init(void);
static unsigned int get_hex(unsigned int num, char * pSum);
static char get_intel_hex(char verify);
static char get_binary(char verify);
static void menu(void);

/**
 * Hardware-level receive buffer/register.
 */
#if defined NC30
#define RXREG u0rb
#else
#define RXREG U2RXREG
#endif

/******************************************************************************/

/**
 * CRC lookup tables.
 */
static const unsigned char crclo[256] = {
   0x00,0x21,0x42,0x63,0x84,0xA5,0xC6,0xE7,0x08,0x29,0x4A,0x6B,0x8C,0xAD,0xCE,0xEF,
   0x31,0x10,0x73,0x52,0xB5,0x94,0xF7,0xD6,0x39,0x18,0x7B,0x5A,0xBD,0x9C,0xFF,0xDE,
   0x62,0x43,0x20,0x01,0xE6,0xC7,0xA4,0x85,0x6A,0x4B,0x28,0x09,0xEE,0xCF,0xAC,0x8D,
   0x53,0x72,0x11,0x30,0xD7,0xF6,0x95,0xB4,0x5B,0x7A,0x19,0x38,0xDF,0xFE,0x9D,0xBC,
   0xC4,0xE5,0x86,0xA7,0x40,0x61,0x02,0x23,0xCC,0xED,0x8E,0xAF,0x48,0x69,0x0A,0x2B,
   0xF5,0xD4,0xB7,0x96,0x71,0x50,0x33,0x12,0xFD,0xDC,0xBF,0x9E,0x79,0x58,0x3B,0x1A,
   0xA6,0x87,0xE4,0xC5,0x22,0x03,0x60,0x41,0xAE,0x8F,0xEC,0xCD,0x2A,0x0B,0x68,0x49,
   0x97,0xB6,0xD5,0xF4,0x13,0x32,0x51,0x70,0x9F,0xBE,0xDD,0xFC,0x1B,0x3A,0x59,0x78,
   0x88,0xA9,0xCA,0xEB,0x0C,0x2D,0x4E,0x6F,0x80,0xA1,0xC2,0xE3,0x04,0x25,0x46,0x67,
   0xB9,0x98,0xFB,0xDA,0x3D,0x1C,0x7F,0x5E,0xB1,0x90,0xF3,0xD2,0x35,0x14,0x77,0x56,
   0xEA,0xCB,0xA8,0x89,0x6E,0x4F,0x2C,0x0D,0xE2,0xC3,0xA0,0x81,0x66,0x47,0x24,0x05,
   0xDB,0xFA,0x99,0xB8,0x5F,0x7E,0x1D,0x3C,0xD3,0xF2,0x91,0xB0,0x57,0x76,0x15,0x34,
   0x4C,0x6D,0x0E,0x2F,0xC8,0xE9,0x8A,0xAB,0x44,0x65,0x06,0x27,0xC0,0xE1,0x82,0xA3,
   0x7D,0x5C,0x3F,0x1E,0xF9,0xD8,0xBB,0x9A,0x75,0x54,0x37,0x16,0xF1,0xD0,0xB3,0x92,
   0x2E,0x0F,0x6C,0x4D,0xAA,0x8B,0xE8,0xC9,0x26,0x07,0x64,0x45,0xA2,0x83,0xE0,0xC1,
   0x1F,0x3E,0x5D,0x7C,0x9B,0xBA,0xD9,0xF8,0x17,0x36,0x55,0x74,0x93,0xB2,0xD1,0xF0
};

static const unsigned char crchi[256] =
{
   0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x81,0x91,0xA1,0xB1,0xC1,0xD1,0xE1,0xF1,
   0x12,0x02,0x32,0x22,0x52,0x42,0x72,0x62,0x93,0x83,0xB3,0xA3,0xD3,0xC3,0xF3,0xE3,
   0x24,0x34,0x04,0x14,0x64,0x74,0x44,0x54,0xA5,0xB5,0x85,0x95,0xE5,0xF5,0xC5,0xD5,
   0x36,0x26,0x16,0x06,0x76,0x66,0x56,0x46,0xB7,0xA7,0x97,0x87,0xF7,0xE7,0xD7,0xC7,
   0x48,0x58,0x68,0x78,0x08,0x18,0x28,0x38,0xC9,0xD9,0xE9,0xF9,0x89,0x99,0xA9,0xB9,
   0x5A,0x4A,0x7A,0x6A,0x1A,0x0A,0x3A,0x2A,0xDB,0xCB,0xFB,0xEB,0x9B,0x8B,0xBB,0xAB,
   0x6C,0x7C,0x4C,0x5C,0x2C,0x3C,0x0C,0x1C,0xED,0xFD,0xCD,0xDD,0xAD,0xBD,0x8D,0x9D,
   0x7E,0x6E,0x5E,0x4E,0x3E,0x2E,0x1E,0x0E,0xFF,0xEF,0xDF,0xCF,0xBF,0xAF,0x9F,0x8F,
   0x91,0x81,0xB1,0xA1,0xD1,0xC1,0xF1,0xE1,0x10,0x00,0x30,0x20,0x50,0x40,0x70,0x60,
   0x83,0x93,0xA3,0xB3,0xC3,0xD3,0xE3,0xF3,0x02,0x12,0x22,0x32,0x42,0x52,0x62,0x72,
   0xB5,0xA5,0x95,0x85,0xF5,0xE5,0xD5,0xC5,0x34,0x24,0x14,0x04,0x74,0x64,0x54,0x44,
   0xA7,0xB7,0x87,0x97,0xE7,0xF7,0xC7,0xD7,0x26,0x36,0x06,0x16,0x66,0x76,0x46,0x56,
   0xD9,0xC9,0xF9,0xE9,0x99,0x89,0xB9,0xA9,0x58,0x48,0x78,0x68,0x18,0x08,0x38,0x28,
   0xCB,0xDB,0xEB,0xFB,0x8B,0x9B,0xAB,0xBB,0x4A,0x5A,0x6A,0x7A,0x0A,0x1A,0x2A,0x3A,
   0xFD,0xED,0xDD,0xCD,0xBD,0xAD,0x9D,0x8D,0x7C,0x6C,0x5C,0x4C,0x3C,0x2C,0x1C,0x0C,
   0xEF,0xFF,0xCF,0xDF,0xAF,0xBF,0x8F,0x9F,0x6E,0x7E,0x4E,0x5E,0x2E,0x3E,0x0E,0x1E
};

static unsigned char crc_l, crc_h, rx_crc_l, rx_crc_h;

/**
 * Calculate CRC on-the-fly as bytes arrive.
 * @param c the current incoming byte
 */
inline void update_crc(unsigned char c)
{
   unsigned i = c ^ (crc_h);

   crc_h = crc_l ^ crchi[i];
   crc_l = crclo[i];
}

/**
 * Set the in index to the out index to force received data to be "none".
 */
static void flush_rx(void)
{
   rx_in = rx_out;
}

/**
 * One of the receive interrupt handler routines.
 * This is used when waiting for text from the interactive user menu presented
 * to Hyperterminal.
 */
static void rx_terminal(void)
{
   unsigned next;
   rx_buffer[rx_in] = RXREG;

   next = (rx_in + 1) & (RX_CIRC_SIZE-1);
   if ( next != rx_out )
   {
      rx_in = next;
   }
}

/**
 * Actual receive and transmit ISRs.
 */
#if defined FLASHPROG && FLASHPROG > 0
#if defined NC30
void uart_receive(void)     // receive ISR
{
   rx_func();
}

void uart_transmit(void)    // transmit ISR (not used)
{
}
#else
/**
 * UART receive interrupt handler: Read incoming byte from receive register,
 * place it into the receive buffer, and increment the incoming data counter
 *  (wrap the counter as necessary).
 */
void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
   rx_func();
   IFS1bits.U2RXIF = 0; /* clear interrupt flag */
}

/**
 * UART transmit interrupt handler: Write outgoing byte to the transmit
 * register and increment the incoming data counter (wrap the counter as
 * necessary).
 */
void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
   IFS1bits.U2TXIF = 0; /* clear interrupt flag */
}
#endif
#endif

/**
 * Initialization of UART registers specific to the target device.
 */
static void uart_setup(void)
{
#if defined NC30
   u0mr = 5;      // 8 bit no parity 1 stop
   u0c0 = 0x10;   // disable handshake
   ucon = 0;
   u0brg = (24000000/16/57600) - 1;     // 57600 baud from 24MHz
   s0tic = 0;      // no transmit interrupts
   s0ric = 1;
   rx_func = rx_terminal;
   u0c1 = 5;
#else
   InitUART2();
   rx_func = rx_terminal;
#endif
}

/**
 * Check whether new data is present in the receive buffer.
 * 
 * @return char
 */
static char is_char(void)
{
   return(rx_in != rx_out) ? 1 : 0;
}

/**
 * Obtain a byte from the received data buffer.
 * 
 * @return char
 */
static char get_char(void)
{
   char rx;
   volatile unsigned int rx_I;
   while ( rx_in == rx_out )
   {
   }
   rx_I = rx_in;                                //bug fix save a copy of the rx_in interrupt index so code below runs on a non interrupt based index
   rx = rx_buffer[rx_out];
   rx_out = (rx_out + 1) & (RX_CIRC_SIZE-1);
   if ( FLOW_CONTROL )            //check if flow control due to input buffer filling up.
   {
      if ( ((rx_out > rx_I) && (rx_out - rx_I > BUFFER_FULL_THRESHHOLD)) || 
           ((rx_I >= rx_out) && (((RX_CIRC_SIZE-rx_I) + rx_out) >= BUFFER_FULL_THRESHHOLD)) )
      {
         put_char(ACK_CODE);
         FLOW_CONTROL = 0;   
      }
   }
   return rx;
}

/**
 * Send a byte to Hyperterminal (does not use interrupts).
 * @param c the data byte to send
 */
static void put_char(char c)
{
#if defined NC30
   for ( ;tiu0c1 == 0; );
   u0tb = c;
#else
   for ( ;U2STAbits.TRMT == 0; );
   U2TXREG = c;
#endif
}

/**
 * Send a string to Hyperterminal.
 * @param pString pointer to the string text
 */
static void put_string(const char * pString)
{
   for ( ; (*pString); pString++ )
   {
      put_char(*pString);
   }
}

/**
 * Check for XMODEM errors.
 * [Not sure if this is code currently being used.]
 * 
 * @return unsigned
 */
static unsigned error_check(void)
{
   if ( ++xmodem_error > MAX_ERROR )
   {
      rx_func = rx_terminal;
      return 1;
   }
   return 0;
}

/**
 * One of the receive interrupt handler routines.
 */
static void rx_xmodem_stx(void)
{
   char ch = RXREG;
   if ( ch == STX_CODE || ch == SOH_CODE )
   {
      xmodemPktSize = 1024;
      if ( ch == SOH_CODE )
      {
         xmodemPktSize = 128;
      }
      crc_l = crc_h = 0;
      rx_func = rx_xmodem_blk;
   }
   else if ( ch == EOT_CODE )
   {
      // All done...
      put_char(ACK_CODE);
      rx_func = rx_terminal;
   }
   else
   {
      if ( !error_check() )
      {
         put_char(NAK_CODE);
      }
   }
}

/**
 * One of the receive interrupt handler routines.
 */
static void rx_xmodem_blk(void)
{
   char ch = RXREG;
   if ( ch == xmodem_block_num )
   {
      rx_func = rx_xmodem_nblk;
   }
   else
   {
      if ( !error_check() )
      {
         put_char(NAK_CODE);
         rx_func = rx_xmodem_stx;
      }
   }
}

/**
 * One of the receive interrupt handler routines.
 */
static void rx_xmodem_nblk(void)
{
   char ch = RXREG;
   if ( ch == ~xmodem_block_num )
   {
      //
      // Init the data storage pointer
      // A separate circular buffer input pointer is maintained
      // until the CRC is verified.
      //
      rx_in_xmodem = rx_in;
      xmodem_count = 0;
      rx_func = rx_xmodem_data;
   }
   else
   {
      if ( !error_check() )
      {
         put_char(NAK_CODE);
         rx_func = rx_xmodem_stx;
      }
   }
}

/**
 * One of the receive interrupt handler routines.
 */
static void rx_xmodem_data(void)
{
   unsigned next;
   char ch;

   ch = RXREG;
   update_crc(ch);
   rx_buffer[rx_in_xmodem] = ch;

   next = (rx_in_xmodem + 1) & (RX_CIRC_SIZE-1);
   if ( next != rx_out )
   {
      rx_in_xmodem = next;
      if ( ++xmodem_count >= xmodemPktSize /*1024*/ )
      {
         rx_func = rx_xmodem_crc1;
      }
   }
   else
   {
      //
      // this is really our fault, for some reason buffer has overflowed...
      //
      if ( !error_check() )
      {
         put_char(NAK_CODE);
         rx_func = rx_xmodem_stx;
      }
   }
}

/**
 * One of the receive interrupt handler routines.
 */
static void rx_xmodem_crc1(void)
{
   rx_crc_h = RXREG;
   rx_func = rx_xmodem_crc2;
}


/**
 * One of the receive interrupt handler routines.
 */
static void rx_xmodem_crc2(void)
{
   rx_crc_l = RXREG;
   //
   // The resulting crc should be zero.
   //
   if ( crc_l == rx_crc_l && crc_h == rx_crc_h )
   {
      /**
       * Good block received, prepare to receive the next.
       * Release the data to the input buffer by copying the xmodem circular
       * pointer to the main pointer.
       */
      rx_in = rx_in_xmodem;
      ++xmodem_block_num;
      rx_func = rx_xmodem_stx;
      /** Hold back the ACK if the foreground is falling behind... */
      if ( (rx_out > rx_in && (rx_out-rx_in) < BUFFER_FULL_THRESHHOLD) || ((rx_in > rx_out && ((RX_CIRC_SIZE-rx_in) + rx_out) < BUFFER_FULL_THRESHHOLD)) )
      {
         FLOW_CONTROL = 1;
      }
      else
      {
         put_char(ACK_CODE); 
      }   
   }
   else
   {
      if ( !error_check() )
      {
         put_char(NAK_CODE);
         rx_func = rx_xmodem_stx;
      }
   }        
}

static void xmodem_receive_init(void)
{
   unsigned long i;
   //
   // initialize the crc calculator
   //
   xmodem_error = 0;
   xmodem_block_num = 1;
   rx_func = rx_xmodem_stx;

   for ( i = 0; rx_func == rx_xmodem_stx; ++i )
   {
      if ( (i & 0x7ffff/* 0x7fff*/) == 0 )  //slow the sending of 'C' down a bit
      {
         put_char(GO_CODE);
      }
   }
}

/**
 * Receive the binary file and write to the DataFlash or verify previously
 * written data.
 * 
 * @param verify if verify is set, do not write to DataFlash -- instead read and
 *               compare to previsously written data.
 * 
 * @return char
 */
static char get_binary(char verify)
{
   static char pBuffer[BIN_CHUNK];
   static char pRead[BIN_CHUNK];
   unsigned long address = 0;
   unsigned int i;
   char result = 1;

   while ( rx_func != rx_terminal || is_char() )
   {
      //!!!! bug !!! this can cause a rite/verify failure... depends on timing
      //while (! is_char())  
      //{
      //    if (rx_func == rx_terminal)
      //    {
      //        flash_write_flush();
      //        return result;
      //    }
      //}

      // Can get back here before receive mode changes to rx_terminal...
      for ( i = 0; i < BIN_CHUNK; ++i )
      {
         pBuffer[i] = get_char();
      }
      // Ignore data beyond size of flash
      //
      if ( START_OF_FLASH + address < FLASH_SIZE )
      {
         i = BIN_CHUNK;
         if ( START_OF_FLASH + address + BIN_CHUNK > FLASH_SIZE )
         {
            i = (FLASH_SIZE - address);
         }
         if ( verify )
         {
            flash_read(START_OF_FLASH + address, pRead, i);
            if ( memcmp(pBuffer, pRead, i) != 0 )
            {
               result = 0;

               asm volatile("disi #100");
               rx_func = rx_terminal;
               break;  //added TBD 06
            }
         }
         else
         {
            flash_rewrite(address, pBuffer, i);
         }
      }
      address += BIN_CHUNK;
   }
   flash_write_flush();
   return result;
}

static void menu(void)
{
   char command = '?';
   char verify = 0;
   char ok;
   static uint8_t index = 0;
   static uint8_t mfg[8];
   static uint8_t str[] = "\r\nxxxxxxxx\r\n";
   //uint8_t id[3];

   for ( ;; )
   {
      ok = 1;
      switch ( command )
      {
      case '?':
         put_string("\r\n"
                    "GE Flash Programmer\r\n" 
                    "Commands:\r\n"
                    "   A-Erase all\r\n"
                    //"   E-Erase\r\n"
                    //"   I-Receive XMODEM transfer of Intel Hex\r\n"
                    "   B-Receive XMODEM transfer of binary file\r\n"
                    "   V-Toggle verify mode\r\n"
                    "   Special chars to program eight hex nybbles of mfg data\r\n"
                    "        ~  `  !  @  #  $  %  ^  &  *  (  )  _  -  +  =\r\n"
                    "        0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n"
                    "   Q-Display mfg section data as eight hex nybbles\r\n"
                    "\n");
         break;

      case 'a':
      case 'A':
         put_string("Erasing All...");
         flash_erase();
         put_string(" Done\r\n\n");
         START_OF_FLASH = 0; //must program entire flash
         break;         
//    case 'I':
//    case 'i':
//    	put_string("Begin XMODEM transfer of Intel hex file...\r\n");
//    	xmodem_receive_init();
//    	ok = get_intel_hex(verify);
//    	flush_rx();
//    	put_string("\r\nXMODEM complete\r\n\n");
//         if (verify)
//          {
//              if (ok)
//              {
//                  put_string("Verify Ok\r\n\n");
//              }
//              else
//              {
//                  put_string("Verify Fail\r\n\n");
//              }
//          }
//    	break;

      case 'B':
      case 'b':
         //address = 0;
         put_string("Begin XMODEM transfer of binary file...\r\n");
         xmodem_receive_init();
         ok = get_binary(verify);
         flush_rx();
         put_string("\r\nXMODEM complete\r\n\n");
         if ( verify )
         {
            if ( ok )
            {
               put_string("Verify Ok\r\n\n");
            }
            else
            {
               put_string("Verify Fail\r\n\n");
            }
         }
         break;

      case '`':
         mfg[index++] = 0;
         break;   
      case '~':
         mfg[index++] = 1;
         break;   
      case '!':
         mfg[index++] = 2;
         break;   
      case '@':
         mfg[index++] = 3;
         break;   
      case '#':
         mfg[index++] = 4;
         break;   
      case '$':
         mfg[index++] = 5;
         break;   
      case '%':
         mfg[index++] = 6;
         break;   
      case '^':
         mfg[index++] = 7;
         break;   
      case '&':
         mfg[index++] = 8;
         break;   
      case '*':
         mfg[index++] = 9;
         break;   
      case '(':
         mfg[index++] = 10;
         break;   
      case ')':
         mfg[index++] = 11;
         break;   
      case '-':
         mfg[index++] = 12;
         break;   
      case '_':
         mfg[index++] = 13;
         break;   
      case '+':
         mfg[index++] = 14;
         break;   
      case '=':
         mfg[index++] = 15;
         break;
    
      case 'q': // read and display manufacturing data (4 bytes) in hex format
      case 'Q':
         flash_read(0x420L, &mfg, 4);
         
         str[2] = mfg[0] / 16;
         str[3] = mfg[0] % 16;
         str[4] = mfg[1] / 16;
         str[5] = mfg[1] % 16;
         str[6] = mfg[2] / 16;
         str[7] = mfg[2] % 16;
         str[8] = mfg[3] / 16;
         str[9] = mfg[3] % 16;
         
         for (index = 2; index < 10; index++)
         {
            if (str[index] <= 9)
               str[index] += '0';
            else
            {
               str[index] -= 0x0A;
               str[index] += 'A';
            }
         }              
         index = 0;
         
         put_string(str);
         break;
                  
//    case 'P':
//    case 'p':
//    
//     Intel hex as a text transfer
//    
//    	put_string("Begin xfer of Intel hex...\r\n");
//    	ok = get_intel_hex(verify);
//    	put_string("Intel hex complete\r\n\n");
//          if (verify)
//          {
//              if (ok)
//              {
//                  put_string("Verify Ok\r\n\n");
//              }
//              else
//              {
//                  put_string("Verify Fail\r\n\n");
//             }
//          }
//    	break;
      
//    case 'E':
//    case 'e':
//       {
//          unsigned i;
//          put_string("Erasing...");
//          for ( i = START_OF_FLASH/FLASH_PAGE_SIZE;i < FLASH_NUM_PAGES;i++ )
//             flash_page_erase(i * (unsigned long)FLASH_PAGE_SIZE);
//     flash_erase();
//          put_string(" Done\r\n\n");
//       }
//    	break;

      case 'V':
      case 'v':
         verify = ! verify;
         if ( verify )
         {
            put_string("Verify On\r\n\n");
         }
         else
         {
            put_string("Verify Off\r\n\n");
         }
         break;

#if 0
      case 'T':
      case 't':
         flash_read(START_OF_FLASH + 260, pRead, 64);        
         flash_read(START_OF_FLASH + 300,  pRead, 64);
         flash_read(START_OF_FLASH + 680, pRead, 64);
         flash_read(START_OF_FLASH + 800, pRead, 64);
         flash_read(START_OF_FLASH + 1000, pRead, 64);

         break;            
#endif
      default:
         put_string("???\r\n");
         break;
      }

      if (index >= 8)   // increase limit for more data
      {
          index = 0;
          //convert and write
          mfg[0] = mfg[0] * 16 + mfg[1];
          mfg[1] = mfg[2] * 16 + mfg[3];
          mfg[2] = mfg[4] * 16 + mfg[5];
          mfg[3] = mfg[6] * 16 + mfg[7];
          flash_rewrite(0x420L, &mfg, 4);
          put_string("\r\nMFG prog done\r\n");
      };
  

      put_string("GE>");
      command = get_char();
   }
}

#ifdef testing_with_emulator
void test(void)
{
   char c;

   flash_read(500, &c, 1);
   flash_rewrite(500, &c, 1);
   //
   // Rewriting a byte will result in a 'dirty' sector.
   // The next two calls to flash_maintenance will rewrite
   // pages zero and one of the flash since 500 lies in page 2 -
   // the first sector of 8 pages.
   //
   flash_maintenance();
   flash_maintenance();

   //
   // Set up two dirty pages.  Currently the page address to rewrite is 2 (unless modified
   // in a watch window).

   flash_read(500, &c, 1);
   flash_rewrite(500, &c, 1);
   flash_read(8*264 + 500, &c, 1);
   flash_rewrite(8*264 + 500, &c, 1);

   //
   // Use a watch window to push the rewrite_page ahead to page 500,
   // see that the algorithm rewindes the rewrite_page back to zero to continue
   // the circular pattern.
   // Push the rewrite_page to 7 to see that sector (bit) zero of the sector_dirty_map
   // gets cleared indicating that the sector has been completely re-written.
   //

   flash_maintenance();
   flash_maintenance();
   flash_maintenance();
   flash_maintenance();
   flash_maintenance();
   flash_maintenance();
   flash_maintenance();
}
#endif

void flashprog(void)
{
#if defined NC30
   // Enable the PLL clock
   // 4 Mhz input multiplied by 6 for 24 Mhz clock
   // 
/*==================================================*/
/*= Procedure to Use PLL Clock as CPU Clock Source =*/
/*==================================================*/
   prcr = 0x03;                    /* Protect off */

   cm0 = 0x08;                    /* Main clock */
   cm1 = 0x20;                    /* No division mode */
   plc0 = 0x13;                   /* Multiply-by-6 */
   pm2 = 0x00;                    /* 2 waits */
   plc0 = 0x93;                   /* PLL on */

   //
   // Note: at this point the cpu is running at 4Mhz
   //
   ta0mr = 0x80;                   /* f32 */
   ta0 = (4000000/32 * 20) / 1000 - 1;   /* 20ms 4MHz */
   ta0ic = 0x00;                   /* Level 0 */
   ta0s = 1;                       /* Timer start */

   while ( ir_ta0ic == 0 )
   {           /* about 20ms wait  Vcc = 5V */
   }                               /* about 50ms wait  Vcc = 3V */
   ta0s = 0;                       /* Timer stop */
   ir_ta0ic = 0;                   /* Interrupt request bit clear */

   cm1 = 0x22;                     /* PLL clock */

   prcr = 0x00;                    /* Protect on */
#endif

   flash_setup();
   uart_setup();
#if defined NC30
   asm(" FSET I");
#endif

#ifdef testing_with_emulator
   test();
#endif
   menu();
}

/**
 * Act as a standalone executable if FLASHPROG is #defined and greater than 0.
 * Otherwise the functions in this module may be used as part of another
 * executable.
 */
#if defined FLASHPROG && FLASHPROG > 0
int main(void)
{
   //FLOW_CONTROL = 1;
   flashprog();
   return 0;
}
#endif

