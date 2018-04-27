/*
 * File:   KeypadThread.c
 * Author: Rick
 *
 * Created on April 26, 2018, 8:32 AM
 * 
 * This thread uses uart4 to talk to the cap touch controller
 * 
 */


#include "xc.h"

#include "mcc_generated_files/uart4.h"
#include "threads.h"
#include "queue.h"
#include "KeypadThread.h"
#include "UIthread.h"




static void Keypad_thread(void);
extern struct Message_Data event_msg;

#define Keypad_QSIZE 10
static struct message_Q Keypad_message_Q;				//container information... number of messages in que.
static struct Message_Data Keypad_Q[Keypad_QSIZE]; //declare message storage array
	
extern struct SYS_STATUS sys_Keypad;


/**
 *
 * create the Keypad manager thread
 *
 */
void Keypad_thread_init(void)
{
   	static THREAD Keypad_thread_struct;
   	static uint8_t Keypad_thread_stack[1000];    // TBD determine needed size 
    
    //Keypad thread initialize();
	thread_create(Keypad_thread, &Keypad_thread_struct, Keypad_thread_stack, sizeof(Keypad_thread_stack));
}

/**
 *
 * place a message in the Keypad mangers inbox
 *
 */
void Keypad_message_put(struct Message_Data *new_msg)
{
	message_Q_put(&Keypad_message_Q, new_msg);
}

/**
 *
 * get a message from the Keypad mangers inbox
 *
 */
struct Message_Data *Keypad_message_get(void)
{
    return message_Q_get(&Keypad_message_Q);
}

/**
 *
 * Initialize the Keypad manager message q
 *
 */
void Keypad_message_init(void)
{
	message_Q_initialize(&Keypad_message_Q,Keypad_Q, Keypad_QSIZE);    	//initialize message q.    
}



//*********************************************************Keypad thread **********************************************

/**
 *
 * Keypad manager thread - never returns
 *
 */
static void Keypad_thread(void)
{
       
   struct Message_Data *pevent_msg;

   for(;;)								//threads should not return
   {

        
        context_switch();	
    }         
}


