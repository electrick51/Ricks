#include "twttsdef.h"
#include "uart2.h"
#include "dataflash.h"

#if defined BoardRev && BoardRev != exp16
// config register macros should appear once
// near top of a .C file and outside of any function
//
// JTAG/Code Protect/Write Protect/Clip-on Emulation mode
// Watchdog Timer/ICD pins select

// note: Explorer 16 board uses ICS_PGx2
//_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)

#if defined FLASHPROG && FLASHPROG > 0
_CONFIG1(JTAGEN_OFF & GCP_ON & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1)
#elif defined WDT_ENABLED && WDT_ENABLED > 0    // 4-second watchdog timer
_CONFIG1(JTAGEN_OFF & GCP_ON & GWRP_OFF & COE_OFF & FWDTEN_ON & WDTPS_PS1024 & ICS_PGx1)
#else
_CONFIG1(JTAGEN_OFF & GCP_ON & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1)
#endif
// note: use ICS_PG2 to avoid a/d converter silicon bug
//_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)

// Disable CLK switch and CLK monitor, OSCO or Fosc/2, HS oscillator,
// Primary oscillator
//_CONFIG2(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_XT & FNOSC_PRI)
//_CONFIG2(OSCIOFNC_OFF & POSCMOD_XT & FNOSC_PRIPLL & IESO_ON & FCKSM_CSECMD & IOL1WAY_OFF)

// after discussion with Jim Ziemer Microchip FAE - gmk
// set IESO_OFF and FCKSM to disable clock switching and clock monitor
_CONFIG2(OSCIOFNC_OFF & POSCMOD_XT & FNOSC_PRIPLL & IESO_OFF & IOL1WAY_OFF)

#endif



void systemInit(void)
{
#ifndef SQI_FLASH
    //L_FLASH_RS=0;   // start the serial flash memory chip from reset
    //T_FLASH_RS=0;
#endif    

// setup peripheral pin select
// unlock the PPS
   asm volatile ( "mov #OSCCON,w1 \n"
               "mov #0x46, w2 \n"
               "mov #0x57, w3 \n"
               "mov.b w2,[w1] \n"
               "mov.b w3,[w1] \n"
               "bclr OSCCON, #6");

// assign functions to RP pins
// (RP pins support remappable output or input functions)
// example

// RPOR12bits.RP24R = SCK2OUT_IO;      //RP24 = SCK2
// or more concisely
// _RP24R = SCK2OUT_IO;

// note - RP output pins take over the port bit
//        so the TRIS and LAT bits are irrelevant

// assign RPI pins to functions
// (RPI pins support remappable input functions)
// example
// RPINR19bits.U2RXR = 19;             //U2RX = RPI19
// or _U2RCR = 19;
// note - set the port TRIS bits for RPI pins to input mode
//        RPI19 refers to same pin as RP19

/** Set up UART1 as interface to main board. */
   _TRISB7=1;       // rb7/rp7 pin to input
   _U1RXR=7;        // rp7 assigned to rxd1 in
   _LATB8=1;        // B8 set to output
   _TRISB8=0;       // high so as to be in correct state
                    // when uart1 is disabled
   _RP8R=U1TX_IO;   // rp8/txd1 (pin 21) to J1-7 ---> EBRD_RXD (pin 62)

// rp11 used for voice pwm output  (not used in PTS version)
// RPOR5bits.RP11R = OC1_IO;				//assign pin 72 it pwm output
//  _RP11R = OC2_IO;

    //   _RP17R = OC4_IO;     // LED PWM  not used in PTS version

   //flash_setup();

//   InitUART2();

// rpi pin for rf receiver input data
#if BoardRev==exp16
// for explorer 16 board
  _IC1R = 34;   // RF_RX_DATA - IC1 = RPI34/RE9
#else
//   _IC1R = RPRXDATA;
#endif

// Lock the PPS
   asm volatile ("mov #OSCCON,w1 \n"
                 "mov #0x46, w2 \n"
                 "mov #0x57, w3 \n"
                 "mov.b w2,[w1] \n"
                 "mov.b w3,[w1] \n"
                 "bset OSCCON, #6");
#if 0
// init rf transmitter pins
   L_RF_TX_EN=0;       // output latch=0 --> tx osc off
   T_RF_TX_EN=0;       // set to output mode

//   L_RF_TX_DATA=0;      // output latch=0  --> tx amp off
//   T_RF_TX_DATA=0;      // set to output mode

   // init rf receiver pins
   // max1473 chip pwr
   L_RF_RX_EN=0;        // pwr off
   T_RF_RX_EN=0;        // output

   L_RF_RX_AGCDIS=0;    //
   T_RF_RX_AGCDIS=0;    // output

   L_RF_RX_DFFB=1;      //
   T_RF_RX_DFFB=0;      // output
   O_RF_RX_DFFB=1;      // enable open drain
//   L_RF_RX_DATA=0;      // output latch=0
  // T_RF_RX_DATA=1;      // set to input mode
#endif

   AD1PCFGL=0xffff;     // start with all analog pins in digital mode

    L_SCREEN_EN = 0;    // disable TFT & controller 3.3V supply
    T_SCREEN_EN = 0;    // output

    _LATC14=0;          // disable TFT chip 1.8v supply
    _TRISC14=0;         // output
    //T_BATT_VOLT=1;      // battery voltage pin to input
    //T_SUPPLY_VOLT=1;    // supply voltage pin to input

    //L_BATT_TEST=0;      // batt test off
    //T_BATT_TEST=0;      // batt test pin to output

#if 0
// test code for crc8
void testCrc8(void);
testCrc8();
#endif

#if 0
// test code for crc16
void testCrc16(void);
testCrc16();
#endif
}
/*
RG3
RG2

RF6
RF5
RF3
RF2
RF1

RD6
RD3
RD0

RC13

RB14
RB13
RB12
RB9
RB6
RB5
RB3
RB2
*/
void init_unused_pins(void)
{
#ifndef SQI_FLASH    
    // RG3, RG2
    //LATG&=0XFFF3;
    //TRISG&=0XFFF3;
#endif    

    // RF6, RF5, RF3, RF2, RF1
    LATF&=0XFF91;
    TRISF&=0XFF91;

    // RD6, RD3, RD2, RD0
     LATD&=0XFFB2;
    TRISD&=0XFFB2;  // 11111111 10110010

    // RC13
    LATC&=0XDFFF;
    TRISC&=0XDFFF;

    // RB14, RB13, RB12, RB9, RB6, RB5, RB3, RB2
    LATB&=0X8D93;
    TRISB&=0X8D93;

    // RB1,RB0 (used by ICE emulators)
    LATB&=0XFFFC;
    TRISB&=0XFFFC;  // 11111111 11111100

    // Set unused pins to inputs
    
    TRISDbits.TRISD5=0x01;  // Pin 54   RD5
    LATDbits.LATD5 = 0x01;
    TRISDbits.TRISD3=0x01;  // Pin 51   RD3
    LATDbits.LATD3 = 0x01;
    TRISDbits.TRISD0=0x01;  // Pin 46   RD0
    LATDbits.LATD0 = 0x01;
    TRISBbits.TRISB5=0x01;  // Pin 11   RB5
    LATBbits.LATB5 = 0x01;
    TRISBbits.TRISB3=0x01;  // Pin 13   RB3
    LATBbits.LATB3 = 0x01;
    TRISBbits.TRISB2=0x01;  // Pin 14   RB2
    LATBbits.LATB2 = 0x01;
    TRISBbits.TRISB6=0x01;  // Pin 17   RB6
    LATBbits.LATB6 = 0x01;
    
    
    // Enable pull-ups for unused pins
    
    CNPU1bits.CN15PUE=0x01;  // Pin 54
    CNPU4bits.CN52PUE=0x01;  // Pin 51
    CNPU4bits.CN49PUE=0x01;  // Pin 46
    CNPU1bits.CN7PUE=0x01;  // Pin 11
    CNPU1bits.CN5PUE=0x01;  // Pin 13
    CNPU1bits.CN4PUE=0x01;  // Pin 14
    CNPU2bits.CN24PUE=0x01;  // Pin 17


            
}

