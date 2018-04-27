
/**
 *
 * Timers (Header File)
 *
 * Description:
 *   This file contains function prototypes that are used in Timers.c
 *
 */
#ifndef TIMERS_H
#define TIMERS_H

#include "queue.h"
#include "exactint.h"

struct timer
{
   uint16_t seconds;
   uint16_t milliseconds;
};
	
extern volatile struct timer Timer;

//Function Prototypes //
void TickInit( void );
uint16_t timers_seconds_get(void);
uint16_t timers_seconds_elapsed(uint16_t start);
uint16_t timers_milliseconds_elapsed(uint16_t start);
uint16_t timers_milliseconds_get(void);

#endif /* TIMERS_H */
