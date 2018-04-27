#include "twttsdef.h"			
#include "Timers.h"
#include "threads.h"


/******************************************************************************
 * File Name:       Timers.c
 * Dependencies:    None
 * Processor:       PIC24FJ256GB110
 * Compiler:        C30 v3.02c
 * Company:         Microchip Technology, Inc.
 *******************************************************************************/
#define STOP_TIMER_IN_IDLE_MODE     0x2000
#define TIMER_SOURCE_INTERNAL       0x0000
#define TIMER_ON                    0x8000
#define GATED_TIME_DISABLED         0x0000
#define TIMER_16BIT_MODE            0x0000
#define TIMER_PRESCALER_1           0x0000
#define TIMER_PRESCALER_8           0x0010
#define TIMER_PRESCALER_64          0x0020
#define TIMER_PRESCALER_256         0x0030
//#define TIMER_INTERRUPT_PRIORITY    0x1000

/****************************************************************************
  Function:
    void TickInit( void )

  Description:
    This function sets up Timer 4 to generate an interrupt every 1 ms.  This
	is the basic timer tick for the system.  The CTMU samples each channel 
	at a 1ms rate.  This yields 16ms to sample all of the channels.  This 
	timer is then disabled while the data gathered is updated by the main 
	program loop.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    The timer period and prescaler 
	values are dependent on the oscillator speed.
  ***************************************************************************/
#define TIMER_PRESCALER		TIMER_PRESCALER_8	//divide by 8
#define TIMER_PERIOD		2000; //@ 8MHz clock with 4x PLL = 32MHz
								  // 16MHz instruction cycle 1/16000000 * 8x
								  // prescale * 2000 = 1mS

volatile struct timer Timer = {0};

void TickInit( void )
{
    TMR4 = 0;
    PR4 = TIMER_PERIOD;
    T4CON = TIMER_ON | TIMER_SOURCE_INTERNAL |
            GATED_TIME_DISABLED | TIMER_16BIT_MODE | TIMER_PRESCALER;

    _T4IF = 0;              //Clear flag
    _T4IE = 1;              //Enable interrupt
    _TON = 1;              //Run timer
}

/****************************************************************************
  Function:
    void __attribute__((interrupt, shadow, auto_psv)) _T4Interrupt(void)

  Description:
    This function calls ReadCTMU() to monitor the
    touchpads.

  Precondition:
    Timer 4 and the Timer 4 interrupt must be enabled in order for
    this function to execute.  CTMUInit() must be called before
    Timer 4 and the Timer 4 interrupt are enabled.  The tickinit() function
	is called to setup timer 4

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
***************************************************************************/

void __attribute__((interrupt, shadow, auto_psv)) _T4Interrupt(void)
{
    update_thread_timer();
    // Clear flag
    _T4IF = 0;
}


/**
 * Return the current seconds value.
 */
uint16_t timers_seconds_get(void)
{
   return Timer.seconds;
}

/**
 * Return the number of seconds elapsed between the current seconds value and
 * the number of seconds passed as the 'start' parameter.
 * @param start  the number of seconds from a past call
 * @return the number of seconds elapsed
 */
uint16_t timers_seconds_elapsed(uint16_t start)
{
   return Timer.seconds - start; /* use unsigned wraparound */
}

uint16_t timers_milliseconds_elapsed(uint16_t start)
{
    return Timer.milliseconds-start; /* unsigned wraparound */    
}    

uint16_t timers_milliseconds_get(void)
{
    return Timer.milliseconds;
}

