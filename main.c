/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC24 / dsPIC33 / PIC32MM MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - pic24-dspic-pic32mm : v1.45
        Device            :  PIC24FJ1024GA606
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.32
        MPLAB             :  MPLAB X 3.61
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "mcc_generated_files/mcc.h"
#include "leds.h"
#include "lcd.h"
#include "mcc_generated_files/tmr3.h"
#include "threads.h"
#include "statusmanager.h"
#include  "mcc_generated_files/pin_manager.h"
#include "pnlComm.h"
#include "mcc_generated_files/uart1.h"
#include "debugMessages.h"
#include "buttons.h"
#include "UIthread.h"
#include "KeypadThread.h"
#include "ULPKTDEF.H"


uint8_t MessageSent = 0;    // Test variable
struct Message_Data event_msg;
struct Message_Data debugMsg;



/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    static int LoopTimes = 0;
    
   
    threads_initialize();
    
    UI_message_init();
    status_manager_message_init();
    Keypad_message_init();
    DebugMessage_message_init();
    
    
    LCD_Initialize();
    
    LED_Enable (LED_D9);
    LED_Enable (LED_D3);
    LED_Enable (LED_D4);
    LED_Enable (LED_D5);
    LED_Enable (LED_D6);
    LED_Enable (LED_D7);
    LED_Enable (LED_D8);
    
//    LED_On(LED_D9);
    printf( "\f" );   
//    
  // IO_RA3_SetHigh();
    
//    printf("LED 9 is on");
    BUTTON_Enable(BUTTON_S3);
    BUTTON_Enable(BUTTON_S6);
    BUTTON_Enable(BUTTON_S4);
    
    
    
    pnlComm_thread_init();
    status_manager_thread_init();
    UI_thread_init();
    Keypad_thread_init();
    
    DebugMessage_thread_init();
    
    const uint8_t sndByte = 0x3F;
    
    
    uint8_t buffer[37] = "1234567890ABCDEFGHIJKLMNOP\n\r";
//    UART1_Initialize();
//    UART1_WriteBuffer(buffer , sizeof(buffer) );
////    UART1_Write( 0x34);
    
    printf(buffer);
        
//        uint8_t writeBuffer[35] = "1234567890ABCDEFGHIJKLMNOP\n" ;
//        unsigned int numBytes = 0;
//        int  writebufferLen = strlen((char *)writeBuffer);
//        UART3_Initialize();
//        LED_On(LED_D6);
//        while(numBytes < writebufferLen)
//        {    
//            int bytesToWrite = UART3_is_tx_ready();
//            numBytes += UART3_WriteBuffer ( writeBuffer+numBytes, bytesToWrite)  ;
//           
//        }
//        LED_Off(LED_D6);
        
    while (1)
    {
        LED_On(LED_D4);
         if(BUTTON_IsPressed(BUTTON_S3) && MessageSent == 0)
        {
             buildDebugMessage(&debugMsg, "Sending One Key\n\r", 1, 0, "MN");
             debugMsg.tag = msg_debug_message;
             DebugMessage_message_put(&debugMsg);
             context_switch();
//             sendText("Sending One key\n\r", 1, 0);
             thread_wait(30);
            event_msg.tag = msg_keypress;
            event_msg.message_info.uplink_pkt.ulink_pkt_type = type_keypress;
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiOneKey;
            UI_message_put(&event_msg);     
            MessageSent = 1;
//           
            while(MessageSent == 1){context_switch();}
             thread_wait(50);
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiTwoKey;
            UI_message_put(&event_msg);      
            MessageSent = 1;
//            thread_wait(50);
            while(MessageSent == 1){context_switch();}
             thread_wait(50);
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiThreeKey;
            UI_message_put(&event_msg);      
            MessageSent = 1;
//            thread_wait(50);
            while(MessageSent == 1){context_switch();}
             thread_wait(50);
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiFourKey;
            UI_message_put(&event_msg);      
            MessageSent = 1;
             
             
        }
           
         if(BUTTON_IsPressed(BUTTON_S6) && MessageSent == 0)
        {
             buildDebugMessage(&debugMsg, "Sending Disarm Key\n\r", 1, 0, "MN");
             debugMsg.tag = msg_debug_message;
             DebugMessage_message_put(&debugMsg);
             context_switch();
//             sendText("Sending One key\n\r", 1, 0);
             thread_wait(30);
//             thread_wait(10);
             event_msg.tag = msg_keypress;
            event_msg.message_info.uplink_pkt.ulink_pkt_type = type_keypress;
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiDisarmKey;
            UI_message_put(&event_msg);        
//             status_message_put(event_msg);      
            MessageSent = 1;
             while(MessageSent == 1){context_switch();}
             thread_wait(50);
             
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiOneKey;
            UI_message_put(&event_msg);     
            MessageSent = 1;
//           
            while(MessageSent == 1){context_switch();}
             thread_wait(50);
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiTwoKey;
            UI_message_put(&event_msg);      
            MessageSent = 1;
//            thread_wait(50);
            while(MessageSent == 1){context_switch();}
             thread_wait(50);
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiThreeKey;
            UI_message_put(&event_msg);      
            MessageSent = 1;
//            thread_wait(50);
            while(MessageSent == 1){context_switch();}
             thread_wait(50);
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiFourKey;
            UI_message_put(&event_msg);      
            MessageSent = 1;
             
             
        }
           
         if(BUTTON_IsPressed(BUTTON_S4) && MessageSent == 0)
        {
//             sendText("Sending Arm All key\n\r", 1, 0);
             buildDebugMessage(&debugMsg, "Sending Arm All key\n\r", 1, 0, "MN");
             debugMsg.tag = msg_debug_message;
             DebugMessage_message_put(&debugMsg);
             context_switch();
             thread_wait(10);
             event_msg.tag = msg_keypress;
            event_msg.message_info.uplink_pkt.ulink_pkt_type = type_keypress;
            event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = uiArmAllKey;
            UI_message_put(&event_msg);        
//             status_message_put(event_msg);      
            MessageSent = 1;
             
             
        }
           
//        numBytes = 0;
        LoopTimes++;
       if(LoopTimes > 32000)
       {
            
//           sendText("Testing Uart3 Messages\n\r", 1, 0);
           if(MessageSent == 1)
           {
               MessageSent = 0;
           }
//           LED_Toggle(LED_D4);
       
//           event_msg->message_info.uplink_pkt.ulink_pkt_type = type_keypress;
//           event_msg->message_info.uplink_pkt.uplink_pkts.generic_uplink.ulbyte0 = StatusKey;
//           pnlComm_put(event_msg);    
//           UART2_Write( sndByte);
//           writebufferLen = strlen((char *)buffer);
//           UART2_Initialize();
//           while(numBytes < writebufferLen)
//            {    
//                int bytesToWrite1 = UART2_is_tx_ready();
//                numBytes += UART2_WriteBuffer(buffer+numBytes, bytesToWrite1);
//
//            }
//           
//           LoopTimes = 0;
        
       }
        
        LED_Off(LED_D4);
        // Add your application code
        context_switch();
        
    }

    return -1;
}
/**
 End of File
*/