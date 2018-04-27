// Threads.c
// Simple non-preemptive multi-threading OS.
//
//
// Wishlist:
// message queues
//
//
// Macros to enable and disable interrupts
// ENTER_CRITICAL is passed a 16bit automatic variable, the current
// interrupt level and flag is stored there before interrupts are disabled
//
// Changed so interrupts with priority > 3 remain enabled
//
//#include "twttsdef.h"

//#define _SP WREG15					// for pic24 compiler WREG15 is used as the stack pointer


#include <setjmp.h>
//#include "typedefine.h"
//#include "cregdef.h"
//#include "intmacro.h"
#include "threads.h"
//#include "console.h"
#include "timers.h"

//
// local functions
//
static THREAD * remove_running_thread(void);

//
// Default threads:
//
static THREAD timer_thread;
static unsigned char timer_stack[200];
static THREAD main_thread;

//
// Thread list pointers:
//
static THREAD * pRunning = &main_thread;
static THREAD * pWaiting;
volatile unsigned int tic_cnt;

//
// update_thread_timer is called from a system timer interrupt
//
void update_thread_timer(void)
{
    static uint16_t milli;
	++tic_cnt;

//	Timer.milliseconds++;
   	if ( ++milli >= 1000 )
   	{
            milli = 0;
//      Timer.seconds++;
   	}

}

#ifdef DEBUG_STACK_CHECK
struct STACK
{
    unsigned char cnt;
    struct StackData
    {
        unsigned char *start;
        unsigned char *end;
        unsigned int size;
        unsigned int used;
    }stackData[7];          //7 is the number of thread in the system
}stackInfo;

/**
    initializeStack() - used for stack depth checking

    Note 1: stackSize = 0, is used for the main stack and should only happen once at program
                         boot-up.  In this case assign the stack to the machines current stack
                         and assign stackSize = SPLIM-current machine stack.  The total
                         stack size in this case will be a few bytes off as the call to the intializeStack
                         causes machine stack usage. To mitigate this issue when stackSize==0 initializeStack()
                         must be called immediatley after boot-up.

    Note 2: there are currently 7 threads which implies 7 stacks in the c4tp
        stack 0: main system stack
        stack 1: timer_stack[]
        stack 2: ui_manager_thread_stack[]
        stack 3: display_stack[]
        stack 4: status_manager_thread_stack[]
        stack 5: C4BusDriver_thread_stack[]
        stack 6: audio_thread_stack[]
 */
static void initializeStackChecker(unsigned char *stackStart, unsigned int stackSize)
{
    unsigned int i;
    register uint16_t stackPointer asm("w15");   

    if (stackSize == 0)             //true => initializing the main stack and booting-up
    {
        //initialize all stack checking data on boot-up
        stackInfo.cnt = 0;
        for (i = 0;i < sizeof(stackInfo.stackData)/sizeof(struct StackData);i++)
        {
            stackInfo.stackData[i].start = (unsigned char *)0;
            stackInfo.stackData[i].end = (unsigned char *)0;
            stackInfo.stackData[i].size = 0;
            stackInfo.stackData[i].used = 0;
        }
        stackStart = (uint8_t *)stackPointer;//_SP;           //_SP is the machines current stack value
        stackSize = SPLIM - stackPointer;//_SP;    //SPLIM is the maximum stack depth and is setup at link time (basically its depth = unused ram area)
    }
    if (stackInfo.cnt >= sizeof(stackInfo.stackData)/sizeof(struct StackData))  //if true there are too many threads running and you must
    {                                                                           //increase the number of elements in stackData[]
        for (;1;)
        {
            __asm__ volatile ("CLRWDT");
        }
    }
    stackInfo.stackData[stackInfo.cnt].start = stackStart;
    stackInfo.stackData[stackInfo.cnt].end = &stackStart[stackSize-1];
    stackInfo.stackData[stackInfo.cnt].size = stackSize;
    for (i=0; i < stackSize;i++)
    {
        stackStart[i] = 0xff;            //mark stack area as unused
    }
    stackInfo.cnt++;
}

static void stackChecker(void)
{
    unsigned char i;
    unsigned int x;

    for (i = 0;i < stackInfo.cnt;i++)
    {
        for (x = stackInfo.stackData[i].size-1; x && stackInfo.stackData[i].start[x] == 0xff;x--);
        stackInfo.stackData[i].used = x;
    }
}
#endif

//
// semaphore_initialize initializes semaphore
//
void semaphore_initialize(SEMAPHORE *pSemaphore, unsigned int count)
{
    pSemaphore->pList = NULL;
    pSemaphore->count = count;
}

//
// semaphore_get
// Request a semaphore.  First come - first serve, with timeout
// Return status:
//      STATUS_OK - semaphore granted
//      STATUS_TIMEOUT - timed out, semaphore not granted
//
THREAD_STATUS semaphore_get(SEMAPHORE *pSemaphore, unsigned int tics)
{
    THREAD_STATUS status;
    THREAD * pCurrent;
//    int status_register;

    ENTER_CRITICAL(status_register);
    
    if (pSemaphore->count != 0)
    {
        //
        // Semaphore is available, return success
        //
        pSemaphore->count -= 1;
        LEAVE_CRITICAL(status_register);
        return STATUS_OK;
    }

    //
    // Store the tics so when the thread is woken up we know if it's in the
    // waiting list.
    //
    pRunning->wait_tics = tics;
    //
    // Put this thread at the end of the semaphore list.
    //
    pCurrent = pSemaphore->pList;
    if (pCurrent == NULL)
    {
        pSemaphore->pList = pRunning;
    }
    else
    {
        while (pCurrent->pSemaNext != NULL)
        {
            pCurrent = pCurrent->pSemaNext;
        }
        pCurrent->pSemaNext = pRunning;
    }
    pRunning->pSemaNext = NULL;

    if (tics != WAIT_FOREVER)
    {
        // Thread was waiting,
        // semaphore has timeout or released
        //
        status = thread_wait(tics);
        if (status != STATUS_TIMEOUT)
        {
            //
            // Got the semaphore
            //
            pSemaphore->count -= 1;

            //
            // Remove self from head of semaphore list
            //
            pSemaphore->pList = pRunning->pSemaNext;
        }
        else
        {
            //
            // Timed out, remove self from semaphore list.
            //
            pCurrent = pSemaphore->pList;
            if (pCurrent == pRunning)
            {
                // Remove from the head of the list
                pSemaphore->pList = pRunning->pSemaNext;
            }
            else
            {
                while (pCurrent->pSemaNext != pRunning)
                {
                    pCurrent = pCurrent->pSemaNext;
                }
                pCurrent->pSemaNext = pRunning->pSemaNext;
            }
        }
    }
    else
    {
        //
        // Wait forever for the semaphore
        //
        status = setjmp(pRunning->jmp);
        //
        // Status is STATUS_NONE when leaving pRunning,
        // otherwise pRunning has been restored.
        //
        if (status == STATUS_NONE)
        {
            //
            // Remove my thread from running list
            //
            (void)remove_running_thread();
            //
            // Resume another thread in running list
            //
            longjmp(pRunning->jmp, pRunning->status);
            // longjmp doesn't return...
        }
        //
        // Got the semaphore, remove my thread from the head of the semaphore list.
        //
        pSemaphore->count -= 1;
        pSemaphore->pList = pRunning->pSemaNext;
    }
    LEAVE_CRITICAL(status_register);
    return status;
}

//
// semaphore_put
// Returns a semaphore so other threads can use the guarded resource.
//
void semaphore_put(SEMAPHORE *pSemaphore)
{
    THREAD * pCurrent;

//    int status_register;

    ENTER_CRITICAL(status_register);
    //
    // Add one to the semaphore counter
    //
    pSemaphore->count += 1;

    //
    // Wake a thread if waiting...
    //
    pCurrent = pSemaphore->pList;
    if (pCurrent)
    {
        // Remove from semaphore list
        pSemaphore->pList = pCurrent->pSemaNext;
        
        if (pCurrent->wait_tics != WAIT_FOREVER)
        {
            //
            // The thread is waiting, remove it from the pWaiting list
            //
            THREAD *pScan = pWaiting;
            //
            // Before changing the links, fixup the tics for the next 
            // thread in the pWaiting list.
            //
            if (pCurrent->pNext)
            {
                // Add the removed tics to the next thread in the timer list.
                pCurrent->pNext->wait_tics += pCurrent->wait_tics;
            }
            if (pScan == pCurrent)
            {
                // remove the thread from head of list
                pWaiting = pCurrent->pNext;
            }
            else
            {
                while (pScan->pNext != pCurrent)
                {
                    pScan = pScan->pNext;
                }
                pScan->pNext = pCurrent->pNext;
            }
        }
        //
        // Add this thread to the running list, after pRunning thread
        //
        pCurrent->status = STATUS_OK;   // Semaphore granted to this thread
        pCurrent->pNext = pRunning->pNext;
        pRunning->pNext = pCurrent;
    }
    LEAVE_CRITICAL(status_register);
}

//
// thread_wait
// Puts the calling thread to sleep for the specified number of tics.
//
THREAD_STATUS thread_wait(unsigned int tics)
{
    THREAD * pCurrent;
    THREAD_STATUS status;

//    int status_register;

    ENTER_CRITICAL(status_register);
    //
    // setjmp saves the context of the active thread
    //
    status = setjmp(pRunning->jmp);
    //
    // Status is STATUS_NONE when leaving pRunning,
    // otherwise pRunning has been restored.
    //
    if (status == STATUS_NONE)
    {
        //
        // Remove thread from running list
        //
        pCurrent = remove_running_thread();
        //
        // Put myself in the wait list
        // List is sorted soonest-to-wake first, and the tics are the delta
        // from the preceding thread in the list, only one counter need be
        // actually counted at any moment.
        //
        if (pWaiting == NULL)
        {
            pWaiting = pCurrent;
            pCurrent->pNext = NULL;
        }
        else
        {
            THREAD * pScan;
            THREAD * pPrevious = NULL;


            for (pScan = pWaiting; pScan != NULL && tics >= pScan->wait_tics; pScan = pScan->pNext)
            {
                pPrevious = pScan;
                tics -= pScan->wait_tics;
            }
            if (pPrevious != NULL)
            {
                //
                // Insert before pScan
                //
                pCurrent->pNext = pScan;
                pPrevious->pNext = pCurrent;
            }
            else
            {
                //
                // Insert at the head of the list
                //
                pCurrent->pNext = pWaiting;
                pWaiting = pCurrent;
            }
            //
            // Update the following delta by subtracting tics for new preceding 
            //
            if (pCurrent->pNext)
            {
                pCurrent->pNext->wait_tics -= tics;
            }
        }
        pCurrent->wait_tics = tics;
        //
        // Resume another thread in running list
        //
        longjmp(pRunning->jmp, pRunning->status);
        // longjmp doesn't return...
    }
    LEAVE_CRITICAL(status_register);
    return status;
}

//
// timer_thread_func
// A thread the manages thread sleep timing, via the pWaiting thread list.
//
static void timer_thread_func(void)
{
    THREAD * pCurrent;
    int time_stamp;
//    int status_register;
#ifdef DEBUG_STACK_CHECK    
    //
    //stack test timer
    //
    uint16_t stackTestTimer;
    stackTestTimer = timers_milliseconds_get();
#endif
    //
    // Get the current value of the free running clock:
    //
    time_stamp = ELAPSED(0);
    for(;;)
    {
#ifdef DEBUG_STACK_CHECK
        if (timers_milliseconds_elapsed(stackTestTimer) > 500)
        {
            stackChecker();  //KGS remove for production
            stackTestTimer = timers_milliseconds_get();
        }
#endif          
        while (ELAPSED(time_stamp) >= MS_TICS(1))
        {
            time_stamp += MS_TICS(1);
            if (pWaiting != NULL)
            {
                //
                // Decrement the tics for the head of the list
                //
                if (pWaiting->wait_tics != 0)
                {
                    pWaiting->wait_tics -= 1;
                }
                //
                // Flush all zero timers out to the running list
                //
                ENTER_CRITICAL(status_register);
                while (pWaiting->wait_tics == 0)
                {
                    pCurrent = pWaiting;
                    pWaiting = pWaiting->pNext;
                    //
                    // Currently running is the timer thread itself.
                    // Put timer completed threads right after pRunning
                    //
                    pCurrent->pNext = pRunning->pNext;
                    pRunning->pNext = pCurrent;
                    pCurrent->status = STATUS_TIMEOUT;
                    if (pWaiting == NULL)
                    {
                        // reached the end of the linked list
                        break;
                    }
                }
                LEAVE_CRITICAL(status_register);
            }
        }
        context_switch();
    }
}

//
// thread_create
// Creates and calls the entry point for a new thread.
// When a thread function returns, that thread is removed from
// the thread list (pRunning) and is not automatically rescheduled.
//
void thread_create(
    THREAD_FUNC_PTR pEntry,
    THREAD * pData,
    unsigned char * stack,
    unsigned int stack_size)
{
    THREAD * pCurrent;
    register uint16_t stackPointer asm("w15");    
//    int status_register;
    //
    // Save context of the creating thread
    //
    // Status is STATUS_NONE when leaving pRunning,
    // otherwise pRunning has been restored.
    //
    ENTER_CRITICAL(status_register);
#ifdef DEBUG_STACK_CHECK
    initializeStackChecker(stack, stack_size);
#endif      
    if (setjmp(pRunning->jmp) == STATUS_NONE)
    {
        //
        // Insert the new thread into the linked list after the active
        // thread.
        //
        pData->status = STATUS_OK;
        pData->pNext = pRunning->pNext;
        pRunning->pNext = pData;
        pData->pEntry = pEntry;
        pRunning = pData;
        //
        // Main thread's stack is assigned at power up.
        if (pData != &main_thread)
        {
            //
            // Assign SP for new thread, once done the thread is completely
            // initialized and ready to run.
            //
            //_SP = (unsigned int *)(stack);// + stack_size);
            stackPointer = (unsigned int)(stack);// + stack_size);            
        }
        //
        // Call the thread function:, it will run until it's first context_switch
        //
        pCurrent = pRunning;
        LEAVE_CRITICAL(status_register);
        pCurrent->pEntry();
        //
        // If thread returns, remove it from the running list
        //
        ENTER_CRITICAL(status_register);
        (void)remove_running_thread();
        //
        // Resume next thread - the created one is done.
        longjmp(pRunning->jmp, pRunning->status);
        // longjmp doesn't return...
    }
    LEAVE_CRITICAL(status_register);
}

//
// context_switch
// Threads call this function when they want to relinquish the CPU to another thread.
//
void context_switch(void)
{
//    int status_register;
    
//    run_critical();                 //run critical tasks every time contect switch is called
    //
    // setjmp saves the context of the active thread
    //
    // Status is STATUS_NONE when leaving pRunning,
    // otherwise pRunning has been restored.
    //
    ENTER_CRITICAL(status_register);
    if (setjmp(pRunning->jmp) == STATUS_NONE)
    {
        // context has been saved, switch context...
        pRunning->status = STATUS_OK;
        pRunning = pRunning->pNext;
        longjmp(pRunning->jmp, pRunning->status);
        // longjmp doesn't return...
    }
    LEAVE_CRITICAL(status_register);
} 
    
#if 0 //currently not using feature
static CRITICAL_TASK *pCurrent = 0;

//
//
//  Run one critical task per context switch();
//  1) Tasks should not directly invoke a context switch.
//  2) Tasks must be fast ~20mS or less.
//
void run_critical(void)
{
    if(pCurrent)
    {
        pCurrent->pNext->criticalTask();
        pCurrent = pCurrent->pNext;
    }            
}

//
// Add a critical task to the operating system que
// Critical tasks are run before every context switch
// Critical tasks should be limited in duration < 20mS
// 
// Note: 
//  1) critical tasks should be installed during startup().
//  2) At this point there is no mechanism to remove a critical task.      
//
void add_critical_task( CRITICAL_TASK * pData, void (*task)())
{
    DI();
    if(pCurrent == 0)    //check for first added task
    {
        pData->criticalTask = task;
        pCurrent = pData;
        pCurrent->pNext = pData;
    }        
    pData->pNext = pCurrent->pNext;
    pCurrent->pNext = pData;
    pData->criticalTask = task;
    pCurrent = pData;
    EI();
}
#endif

//
// threads_initialize
// Called from main to initialize the thread system.
//
void threads_initialize(void)
{
    // initialize the circular linked list:
    main_thread.pNext = &main_thread;
#ifdef DEBUG_STACK_CHECK
    // initialize thread stack checking
    initializeStackChecker(0, 0);      // KGS remove for production
#endif    
    // create the timer thread:
    thread_create(timer_thread_func, &timer_thread, timer_stack, sizeof(timer_stack));
}

//
// remove_running_thread
// Removes the thread at pRunning from the linked list
// Returns a pointer to the previously running thread.
//
static THREAD * remove_running_thread(void)
{
    THREAD *pCurrent;

    //
    // Go down the list until we find the thread in front of the running thread
    // so we can update it's pNext link.
    //
    for (pCurrent = pRunning; pCurrent->pNext != pRunning; pCurrent = pCurrent->pNext);
    pCurrent->pNext = pRunning->pNext;
    pCurrent = pRunning;
    pRunning = pRunning->pNext;
    return pCurrent;
}
