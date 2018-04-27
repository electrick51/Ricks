#ifndef _TWTTSDEF_H
#define _TWTTSDEF_H
//
// file: TWTTSDEF.H
// Note: Number of vowels in this file's name > 0
//
// header file for
// Two Way Talking Touch Screen
//
////////////////////////////////////////////////////////////////////////
//
// CHIP RESOURCE ALLOCATION
//   RP8 & OC1 used for RF Transmitter - rftrx.c
//   RP7 & IC1 used for Rf receiver - rftrx.c
//   RP11 & OC2 used for speech - audio.c
//   Timer4 used for the thread tic (1ms)
//   UART2 used for data flash
//   RP27 used for data flash  RPIN_SPI
//   RP19 used for data flash RPOUT_SPI
//   RP26 used for data flash RPCLK_SPI
//
//   Graphics Subsystem
//   ------------------
//   Enhanced PWM 6 w/sysclk = Background timebase
//   Enhanced PWM 7 w/sysclk = Backlight switcher clock
//   Enhanded PWM 8 w/sysclk = Backlight switcher PWM level control
//
////////////////////////////////////////////////////////////////////////


// define all interrupt priorities in one place
// is a GOOD THING!
#if 0
#define RF_TX_INTPRIORITY 5
#define RF_RX_INTPRIORITY 6
#endif

#include "EXACTINT.h"

#define DEVICETYPE 0x22
// one byte rrrr.mmmm --> major rev.minor rev
// caution - there is string for displaying
//           the firmware rev on the screen
//           the display string and this define
//           should agree!
#define TSFIRMWAREREV 0x11       // rev = 1.1

//
// clock
#define FOSC 8000000
#define XPLL 4
#define FCPU FOSC*XPLL/2 // instruction cycle clock (DOZE=000)
#define FCY FCPU         //

// watchdog
#define WDT_ENABLED 1   // set this to a non-zero value if you want watchdog enabled


////#define BoardRev exp16
//#define BoardRev 60     // schematic marked Revsion 0_6
//
//#if BoardRev==exp16
//// note-emulation uses PGED2, PGEC2
//#include "P24FJ256GA110.h"
//#else
//// note-emulation uses PGED1, PGEC1 (RB0 & RB1)
//#include "P24FJ256GA106.h"
//#endif

// Peripheral Pin Select Outputs
// maps these functions to outputs
#define NULL_IO          0
#define C1OUT_IO         1
#define C2OUT_IO         2
#define U1TX_IO          3
#define U1RTS_IO         4
#define U2TX_IO          5
#define U2RTS_IO         6
#define SDO1_IO          7
#define SCK1OUT_IO       8
#define SS1OUT_IO        9
#define SDO2_IO         10
#define SCK2OUT_IO      11
#define SS2OUT_IO       12
#define OC1_IO          18
#define OC2_IO          19
#define OC3_IO          20
#define OC4_IO          21
#define OC5_IO          22
#define OC6_IO          23
#define OC7_IO          24
#define OC8_IO          25
// 26 & 27 not used
#define U3TX_IO         28
#define U3RTS_IO        29
#define U4TX_IO         30
#define U4RTS_IO        31
#define SDO3_IO         32
#define SCLK3OUT_IO     33
#define SS3OUT_IO       34
#define OC9_IO          35
#define C3OUT_IO        36


// audio
#define T_AUDIOEN _TRISD6
#define L_AUDIOEN _LATD6

// 4.5 volt power supply
#define T_SCREEN_EN  _TRISF0
#define L_SCREEN_EN  _LATF0
#if 0
// rf transmitter & receiver
// agc disable output port b bit 14
#define L_RF_RX_AGCDIS _LATB14
#define T_RF_RX_AGCDIS _TRISB14

// rx power enable output port b bit 13
#define L_RF_RX_EN _LATB13
#define T_RF_RX_EN _TRISB13

// agc pull down output port b bit 9 (open drain ckt)
#define L_RF_RX_DFFB _LATB9
#define T_RF_RX_DFFB _TRISB9
#define O_RF_RX_DFFB _ODB9

// tx data output port b bit 8 (rp8)
//#define T_RF_TX_DATA _TRISB8 // direction, 1=I for input 0=O for output
//#define L_RF_TX_DATA _LATB8  // output latch
#endif
#if 0
#if BoardRev==exp16

// rx data input port e bit 9 (rpi34)
#define L_RF_RX_DATA _LATE9
#define T_RF_RX_DATA _TRISE9

// tx osc output enable port e bit 8
#define T_RF_TX_EN _TRISE8 // direction, 1=I for input 0=O for output
#define L_RF_TX_EN _LATE8  // output latch

#else

// rx data input port b bit 7 (rp7)
//#define RPRXDATA 7
//#define L_RF_RX_DATA _LATB7
//#define T_RF_RX_DATA _TRISB7

// tx osc output enable port b bit 12
#define T_RF_TX_EN _TRISB12 // direction, 1=I for input 0=O for output
#define L_RF_TX_EN _LATB12  // output latch

#endif
#endif




/** Start of DataFlash I/O */
#if BoardRev==exp16

#define T_FLASH_CS _TRISG0 /** direction, 1=I for input 0=O for output */
#define L_FLASH_CS _LATG0  /** output latch */
#define R_FLASH_CS _RG0    /** read port */

#define T_FLASH_SO _TRISD9 /** direction, 1=I for input 0=O for output */
#define L_FLASH_SO _LATD9  /** output latch */
#define R_FLASH_SO _RD9    /** read port */
#define O_FLASH_SO _ODD9   /** open drain control */


#define T_FLASH_SI _TRISC1 /** direction, 1=I for input 0=O for output */
#define L_FLASH_SI _LATC1  /** output latch */
#define R_FLASH_SI _RC1    /** read port */

#define T_FLASH_CK _TRISD10 /** direction, 1=I for input 0=O for output */
#define L_FLASH_CK _LATD10  /** output latch */
#define R_FLASH_CK _RD10    /** read port */

#define T_FLASH_RS _TRISG1 /** direction, 1=I for input 0=O for output */
#define L_FLASH_RS _LATG1  /** output latch */
#define R_FLASH_RS _RG1    /** read port */

#define RPIN_SPI 38        /** use rpi38 remappable input */
#define RPOUT_SPI _RP4R    /** use rp4 for serail flash output */
#define RPCLK_SPI _RP3R   //* use rp3 for serial flash clock

#else

#define T_FLASH_CS _TRISG6 // RP21
#define L_FLASH_CS _LATG6  // low = serial flash chip selected
#define R_FLASH_CS _RG6

// note on schematic 0_6 signal labeled
// flash_so is data from serial flash to cpu
#define T_FLASH_SO _TRISG9 /** direction, 1=I for input 0=O for output */
#define L_FLASH_SO _LATG9  /** output latch */
#define R_FLASH_SO _RG9    /** read port */
#define O_FLASH_SO _ODG9   /** open drain control */

// note on schematic 0_6 signal labeled
// flash_si is data from cpu to serial flash
#define T_FLASH_SI _TRISG8
#define L_FLASH_SI _LATG8
#define R_FLASH_SI _RG8

#define T_FLASH_CK _TRISG7 /** direction, 1=I for input 0=O for output */
#define L_FLASH_CK _LATG7
#define R_FLASH_CK _RG7

#define T_FLASH_RS _TRISD7
#define L_FLASH_RS _LATD7  //low = reset
#define R_FLASH_RS _RD7

#define RPIN_SPI 27        /** use rpi27 remappable input */
#define RPOUT_SPI _RP19R   /** use rp29 for serial flash output */
#define RPCLK_SPI _RP26R   //* use rp26 for serial flash clock

#endif
/** End of DataFlash I/O */

#if BoardRev!=exp16
#define T_LED_PWM _TRISF5
#define L_LED_PWM _LATF5
#define R_LED_PWM _RF5

#define T_DEBUG_RX _TRISD1  // RP24
#define L_DEBUG_RX _LATD1
#define R_DEBUG_RX _RD1

#define T_DEBUG_TX _TRISD2  // RP23
#define L_DEBUG_TX _LATD2
#define R_DEBUG_TX _RD2

#define T_BATT_TEST _TRISD3  // RP22
#define L_BATT_TEST _LATD3
#define R_BATT_TEST _RD3

#define T_BATT_VOLT _TRISB2  // RP13/AN2
#define L_BATT_VOLT _LATB2
#define R_BATT_VOLT _RB2

#define T_SUPPLY_VOLT _TRISB3  // RP13/AN3
#define L_SUPPLY_VOLT _LATB3
#define R_SUPPLY_VOLT _RB3

#define T_SOSCI _TRISC13
#define T_SOSCO _TRISC14    // RPI37

/* begin --> DCS -- need to doublecheck/verify */
#define T_USB_PWR_EN  _TRISB5
#define L_USB_PWR_EN  _LATB5

#define T_SCREEN_EN  _TRISF0
#define L_SCREEN_EN  _LATF0
/* end --> DCS -- need to doublecheck/verify */

#endif

/** LCD Pins */
#define L_LCD_RST   _LATD11
#define L_LCD_CS    _LATD10
#define L_LCD_RD    _LATD5
#define L_LCD_WR    _LATD4
#define L_LCD_CD    _LATB4
#define L_LCD_CLK   _LATB15

#define RPLIGHTCLOCK 6
#define T_LIGHT_CLOCK _TRISB6
#define L_LIGHT_CLOCK _LATB6

/*******************************************************************************
 * The following are a few handy general-purpose macros.
 */

/**
 * Macro used to calculate the number of elements of an array.
 * The macro parameter 'x' must be an array name in scope.
 */
#define NELEM(x) (sizeof(x)/sizeof(*x))

/**
 * Macro used to create a bitmask for an an integer or enumeration-type value.
 * The macro expansion will be as follows:
 *   BIT(0) -> 0x01
 *   BIT(1) -> 0x02
 *   BIT(2) -> 0x04
 *   BIT(3) -> 0x08
 *   BIT(4) -> 0x10
 *   BIT(5) -> 0x20
 *   BIT(6) -> 0x40
 *   BIT(7) -> 0x80
 */
#define BIT(x) (1U << (x))

#endif
