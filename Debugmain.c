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
        Device            :  PIC24FJ1024GA610
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

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/uart1.h"
#include "debugMessages.h"


/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    uint8_t writeBuffer[35] = "This is a sent message\n\r" ;
    uint8_t hexBuffer[10];// = {0x15, 0x04, 0x33, 0x25, 0x10};
    uint8_t hxBuffer[10] = {0x44, 0x66, 0x29, 0x32, 0x92};
    
    uint8_t x, i;
    uint8_t sChar[2];
    uint8_t LoopsThrough = 0;
    

    int timeDelay = 0;
    int td2 = 0;
    
    
    

    while (1)
    {
        hexBuffer[0] = 0x15;
        hexBuffer[1] = 0x04;
        hexBuffer[2] = 0x33;
        hexBuffer[3] = 0x25;
        hexBuffer[4] = 0x10;
        
        hxBuffer[0] = 0x53;
                        hxBuffer[1] = 0x44;
                        hxBuffer[2] = 0x41;
                        hxBuffer[3] = 0x15;
                        hxBuffer[4] = 0x24;
                        
        if(++timeDelay > 32000)
        {
            if(++td2 > 50)
            {
                IO_RA3_SetHigh();

//                for(x = 25; x < 55; x++)
//                {
//                    writeBuffer[x] = 0;
//                }
//                writeBuffer[55] = "This is a sent message\n\r" ;
//                for(x = 0; x < 5; x++)
//                {
//                    int scount = sprintf(sChar, "%X", hexBuffer[x]);
//                
//                    strcat(writeBuffer, sChar);
//                    
//                }
                switch(LoopsThrough)
                {
                    case 0:
                        sendText(writeBuffer, TEXT, 0);
                        LoopsThrough++;
                        break;
                    case 1:
                        sendCRLF();
                        LoopsThrough++;
                        break;
                    case 2:
                        sendText(hexBuffer, HEX, 5);
                        LoopsThrough++;
                        break;
                    case 3:
                        sendCRLF();
                        LoopsThrough++;
                        break;
                    case 4:
                        sendText("This is case 4", TEXT, 0);
                        LoopsThrough++;
                        break;
                    case 5:
                        sendCRLF();
                        LoopsThrough++;
                        break;
                    case 6:
                        
                        sendText(hxBuffer, HEX, 5);
                        LoopsThrough++;
                        break;
                    case 7:
                        sendCRLF();
                        LoopsThrough = 0;
                        break;
                        
                }
//                

                
//                sendCRLF();
                

                IO_RA3_SetLow();
                td2 = 0;
            }
            timeDelay = 0;
        }
        
        
        // Add your application code
    }

    return -1;
}
/**
 End of File
*/