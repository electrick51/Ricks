/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef DEBUG_MESSAGES_H
#define DEBUG_MESSAGES_H

//#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>



// TODO Insert declarations
void DebugMessage_thread_init(void);
void DebugMessage_message_put(struct Message_Data *new_msg);
struct Message_Data *DebugMessage_message_get(void);
void DebugMessage_message_init(void);
void buildDebugMessage(struct Message_Data *dbMsg, uint8_t msg[], uint8_t type, uint8_t size, uint8_t mod[] );

#define TEXT 1
#define HEX   2

// Comment a function and leverage automatic documentation with slash star star
void sendText(uint8_t buffer[], uint8_t type, uint8_t length);
uint8_t formatHex(unsigned char *buff, unsigned char *result, uint8_t length);
void sendCRLF(void);



#endif	/* XC_HEADER_TEMPLATE_H */

