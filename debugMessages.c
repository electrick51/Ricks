/*
 * File:   debugMessages.c
 * Author: Rick
 *
 * Created on April 2, 2018, 7:41 AM
 */


#include "xc.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leds.h"
#include  "threads.h"
#include "queue.h"
#include "resources.h"
#include "ULPKTDEF.H"
#include "mcc_generated_files/uart3.h"
#include "debugMessages.h"

// Make this a thread

static void DebugMessage_thread(void);
static const char *p_token_to_text[];
extern struct Message_Data event_msg;

#define DebugMessage_QSIZE 10
static struct message_Q DebugMessage_message_Q;				//container information... number of messages in que.
static struct Message_Data DebugMessage_Q[DebugMessage_QSIZE]; //declare message storage array
	
//extern struct PROTEST_INFO protest_info;           //contains protest data sent by the panel 
extern struct SYS_STATUS sys_DebugMessage;


/**
 *
 * create the DebugMessage manager thread
 *
 */
void DebugMessage_thread_init(void)
{
   	static THREAD DebugMessage_thread_struct;
   	static uint8_t DebugMessage_thread_stack[1000];    // TBD determine needed size 
    
    //DebugMessage thread initialize();
	thread_create(DebugMessage_thread, &DebugMessage_thread_struct, DebugMessage_thread_stack, sizeof(DebugMessage_thread_stack));
}

/**
 *
 * place a message in the DebugMessage mangers inbox
 *
 */
void DebugMessage_message_put(struct Message_Data *new_msg)
{
//	message_Q_put(&DebugMessage_Q,new_msg);
	message_Q_put(&DebugMessage_message_Q, new_msg);
}

/**
 *
 * get a message from the DebugMessage mangers inbox
 *
 */
struct Message_Data *DebugMessage_message_get(void)
{
    return message_Q_get(&DebugMessage_message_Q);
}

/**
 *
 * Initialize the DebugMessage manager message q
 *
 */
void DebugMessage_message_init(void)
{
	message_Q_initialize(&DebugMessage_message_Q,DebugMessage_Q, DebugMessage_QSIZE);    	//initialize message q.    
}



//*********************************************************DebugMessage thread **********************************************

/**
 *
 * DebugMessage manager thread - never returns
 *
 */
static void DebugMessage_thread(void)
{
    
    struct Message_Data *peventMsg;
    
    
    
    for(;;)
    {
         LED_On(LED_D9);
        peventMsg = DebugMessage_message_get();
        if(peventMsg)
        {
//            sendText("Got DB Message\n\r", 1, 0);
            switch(peventMsg->tag)
            {
                case msg_debug_message:
                    sendText(peventMsg->message_info.debugMsg.textBuffer, peventMsg->message_info.debugMsg.type, peventMsg->message_info.debugMsg.size);
                    thread_wait(60);
                    
                    break;
            }
        }
        
        LED_Off(LED_D9);
        context_switch();
    }
}





// Functions

void sendText(uint8_t mbuffer[], uint8_t type, uint8_t length)
{
    
    unsigned int numBytes = 0;
    int  writebufferLen, hexbufferLen;
    unsigned char writeBuffer[30], rsltBuffer[10];
    memset(&rsltBuffer[0], 0, 10);
    
    uint8_t chrBuffer[2];
    uint8_t hxBuffer[(length * 2) + 4];
    uint8_t x, i;
    uint8_t sChar[2];
    
    
    switch(type)
    {
        case TEXT:
            
            writebufferLen = strlen((char *)mbuffer);
            strcpy(writeBuffer, mbuffer);
            
            break;
        case HEX:   //uint8_t hexBuffer[10] = {0x15, 0x04, 0x33, 0x25, 0x10};

            writebufferLen = formatHex(mbuffer, rsltBuffer, length);
            strncpy(writeBuffer, rsltBuffer, writebufferLen);
            break;
    }
    
    UART3_Initialize();
//    IO_RA3_SetHigh();
    while(numBytes < writebufferLen)
    {    
        int bytesToWrite = UART3_is_tx_ready();
        numBytes += UART3_WriteBuffer ( writeBuffer+numBytes, bytesToWrite)  ;
       
    }
    thread_wait(50);
    
  
    
//        IO_RA3_SetLow();
}

uint8_t formatHex(unsigned char buff[], unsigned char result[], uint8_t length)
{
    unsigned char sBuffer[30], sChar[6];
    uint8_t x, writebufferLen;
    
    for(x = 0; x < length; x++)
    {
        sprintf((char*)sChar, "%02X ", (char*)buff[x]);
                
        strcat(sBuffer, sChar);
                    
    }
    writebufferLen = strlen((char *)sBuffer);
    strncpy(result, sBuffer, writebufferLen);
    
    return writebufferLen;
}


void sendCRLF(void)
{
    uint8_t numBytes = 0;
    uint8_t writebufferLen = 0;
    unsigned char writeBuffer[6] = "\n\r";
    
    writebufferLen = strlen((char *)writeBuffer);
    UART3_Initialize();
//    IO_RA3_SetHigh();
    while(numBytes < writebufferLen)
    {    
        int bytesToWrite = UART3_is_tx_ready();
        numBytes += UART3_WriteBuffer ( writeBuffer+numBytes, bytesToWrite)  ;
       
    }
}

void buildDebugMessage(struct Message_Data *dbMsg, uint8_t msg[], uint8_t type, uint8_t size, uint8_t mod[] )
{
    strcpy(dbMsg->message_info.debugMsg.textBuffer, &msg[0]);
    strcpy(dbMsg->message_info.debugMsg.mod, mod);
    dbMsg->tag = msg_debug_message;
    dbMsg->message_info.debugMsg.type = type;
    dbMsg->message_info.debugMsg.size = size;
         
}