//
// Threads.h
// API for treading system
//
#ifndef THREADS_H
#define THREADS_H 1

#include <setjmp.h>
#include <stddef.h>//#define NULL 0

//#define DEBUG_STACK_CHECK //KGS comment out for production

#define ENTER_CRITICAL(save_status)\
__asm__ volatile("disi #0x3FFF"); /* disable interrupts */

//
// LEAVE_CRITICAL is passed the same 16bit variable that was passed to
// ENTER_CRITICAL and restores the previous interrupt level.
//
#define LEAVE_CRITICAL(save_status)\
__asm__ volatile("disi #0x0000"); /* enable interrupts */

//
// The following macro must be customized to the hardware.
// TIC_FREQ is the frequency at which the function update_thread_timer
// is called.
//
#ifndef TIC_FREQ
#define	TIC_FREQ    1000        // hz (actual value doesn't matter as long as it's >= 1000hz)
#endif


// These macros are used by the system to calculate periods:
//
#define MS_TICS(t)  (TIC_FREQ * (t)/1000) // number of counts per millisecond
#define SEC_TICS(t) (TIC_FREQ * (t))      // number of counts per second
#define ELAPSED(x)  (tic_cnt - (x))       // returns the time elapsed since x
#define WAIT_FOREVER 0xFFFF




typedef void (*THREAD_FUNC_PTR)(void);
typedef enum {
    STATUS_NONE = 0, STATUS_OK, STATUS_TIMEOUT 
} THREAD_STATUS;

typedef struct thread_tag
{
    struct thread_tag *pNext;       // Running or waiting list link
    struct thread_tag *pSemaNext;   // Semaphore list link
    unsigned int wait_tics;
    THREAD_STATUS status;
    jmp_buf jmp;
    THREAD_FUNC_PTR pEntry;
} THREAD;

typedef struct critical_tag
{
    struct critical_tag *pNext;
    void (*criticalTask)(void);    
}CRITICAL_TASK;

typedef struct
{
    unsigned int count;
    THREAD * pList;
} SEMAPHORE;

//
// API for threads:
//
void thread_create(
    THREAD_FUNC_PTR pEntry, 
    THREAD * pData,
    unsigned char * stack, 
    unsigned int stack_size);

void context_switch(void);
THREAD_STATUS thread_wait(unsigned int tics);
void threads_initialize(void);
THREAD_STATUS semaphore_get(SEMAPHORE *pSemaphore, unsigned int tics);
void semaphore_put(SEMAPHORE *pSemaphore);
void semaphore_initialize(SEMAPHORE *pSemaphore, unsigned int count);
//
// This function must be called at the rate TIC_FREQ by
// the user application.
// Typically this is done in an interrupt.
//
void update_thread_timer(void);

extern volatile unsigned int tic_cnt;
void run_critical(void);
void add_critical_task( CRITICAL_TASK * pData, void (*task)());
//uint16_t to_seconds_remaining(uint16_t expire_tm);

#endif   /* THREADS_H sentry */
