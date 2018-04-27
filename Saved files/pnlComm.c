/*
 * File:   pnlComm.c
 * Author: Rschneew
 *
 * Created on March 18, 2018, 10:34 AM
 */

#include "xc.h"
#include "TouchscreenFiles/threads.h"
#include "TouchscreenFiles/queue.h"
#include "leds.h"
#include "mcc_generated_files/uart1.h"
#include "TouchscreenFiles/sys_variables.h"
#include "pnlComm.h"
#include "TouchscreenFiles/pnserial.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "debugMessages.h"


#define RF_TX_QSIZE 10

extern uint8_t MessageSent; // Test variable

static void pnlComm_thread(void);
static const char *p_token_to_text[];

#define status_QSIZE 10
static struct message_Q pnlComm_message_Q; //container information... number of messages in que.
static struct Message_Data status_Q[status_QSIZE]; //declare message storage array
uint8_t PnlTxQinx; // input index
uint8_t PnlTxQoutx; // output index

struct Uplink_Pkts PnlTxUQ[RF_TX_QSIZE];

extern struct SYS_STATUS sys_status;
//__TRACE(id);

uint8_t attempts;
uint16_t passcount;
uint16_t failcount;
uint8_t timeout;
uint16_t errors;
uint16_t retries;
uint8_t timeout1;
uint16_t crcerrors1;
uint16_t dataerrors1;
uint16_t counterrors1;
uint16_t retries1;

uint8_t XXX;
uint16_t starttime, datachecktime;
uint16_t calcCrc(uint8_t *data, uint16_t len);




extern uint8_t scratch_buffer[];
#if SCRATCH_BUFFER_SZ < STREAM_BLOCK_SZ            //cause a compiler error if scratch buffer is too small
error scratch buffer too small
#endif 
extern struct SENSORQUE sensorque[];

/**
 *
 * create the status manager thread
 *
 */
void pnlComm_thread_init(void) 
{
    static THREAD pnlComm_thread_struct;
    static uint8_t pnlComm_thread_stack[1000]; // TBD determine needed size 

    //status_manager thread initialize();
    thread_create(pnlComm_thread, &pnlComm_thread_struct, pnlComm_thread_stack, sizeof (pnlComm_thread_stack));
}

/**
 *
 * place a message in the status mangers inbox
 *
 */
void pnlComm_put(struct Message_Data *new_msg) 
{
    //	message_Q_put(&status_Q,new_msg);
    //	message_Q_put(&status_message_Q, new_msg);
    PnlTxUQ[PnlTxQinx] = new_msg->message_info.uplink_pkt;
    sendText("Received a Q message\n\r", 1, 0);
//    sendText(new_msg->message_info.uplink_pkt.uplink_pkts.key_value.keycode, 2, 1);
    thread_wait(30);
    
    if (++PnlTxQinx >= RF_TX_QSIZE) 
    {
        PnlTxQinx = 0;
    }
}

/**
 *
 * get a message from the status mangers inbox
 *
 */
struct Message_Data *pnlComm_message_get(void) 
{
    return message_Q_get(&pnlComm_message_Q);
}

/**
 *
 * Initialize the status manager message q
 *
 */
void pnlComm_message_init(void) 
{
    message_Q_initialize(&pnlComm_message_Q, status_Q, status_QSIZE); //initialize message q.    
}

static void pnlComm_thread(void) 
{
    //*********************************************************
    uint8_t txIndex;
    uint8_t txdBuf [16];
    uint8_t rxIndex;
    uint8_t rxdBuf [16];

    unsigned char rsltBuffer[10];

    memset(&rsltBuffer[0], 0, 10);
    
    uint8_t i;
    uint8_t ptype;
    //       uint8_t txdBuf[10];
    //       uint8_t rxdbuf[10];
    uint16_t tmp;
    //***********************************************************

    LED_Enable(LED_D3);
    LED_Enable(LED_D4);
    LED_Enable(LED_D5);
    LED_Enable(LED_D6);

    static int LoopTimer3 = 0;
    uint8_t numBytes = 20;
    uint8_t buffer[8] = {0x01, 0x22, 0x33, 0x26, 0x01, 0x22, 0x33, 0x26};
    //    UART1_Initialize();
    //    UART1_WriteBuffer(buffer , sizeof(buffer) );



    for (;;) 
    {
        LoopTimer3++;
        if (LoopTimer3 > 16000) 
        {

            //          UART1_WriteBuffer(buffer , sizeof(buffer) );
            //          UART1_Write( 0x34);
            LoopTimer3 = 0;
        }


        //*****************************************************************
        //  Gerry's Tx code from Pnserial.c

        while (PnlTxQoutx < PnlTxQinx) 
        {

//            sendText("Got a packet to send\n\r", 1, 0); // Debug message
//            thread_wait(10);

            // next uplink data code to tx buffer
            txdBuf[0] = 7; // # of bytes to follow
            txdBuf[1] = PTS_KEYCMD;
            ptype = PnlTxUQ[PnlTxQoutx].ulink_pkt_type;
            ptype &= 0x0f;
            txdBuf[2] = ptype;
            //
            switch (ptype) 
            {
                case type_keypress:
//                    sendText("Type = KEYPRESS\n\r", 1, 0); // Debug message
//                    thread_wait(10);

                case type_cmd:
                    txdBuf[3] = PnlTxUQ[PnlTxQoutx].uplink_pkts.key_value.keycode;
                    txdBuf[4] = 0; // was tx trigger count
                    txdBuf[5] = 0; // was tx trigger count
                    break;

//                    sendText(txdBuf[3], 2, 1);
//                    thread_wait(30);
//                    sendCRLF();
//                    thread_wait(20);
                    
                default:
                case type_streamack:
                    txdBuf[3] = PnlTxUQ[PnlTxQoutx].uplink_pkts.generic_uplink.ulbyte0;
                    txdBuf[4] = PnlTxUQ[PnlTxQoutx].uplink_pkts.generic_uplink.ulbyte1;
                    txdBuf[5] = PnlTxUQ[PnlTxQoutx].uplink_pkts.generic_uplink.ulbyte2;
                    break;
            }
            tmp = calcCrc(txdBuf, 6); //crc over 6 bytes
            txdBuf[6] = (uint8_t) tmp; //save the msb ofthe crc
            txdBuf[7] = (uint8_t) (tmp >> 8); //save lsb of crc

            int writebufferLen = 8;

            // packet to send
            // txdBuf[0]= # of following bytes = 7
            // txdBuf[1]= PTS_KEYCMD
            // txdBuf[2]= ptype (type keypress or cmd or ...)
            // txdBuf[3]= key or cmd or data
            // txdBuf[4]= trigger count or data
            // txdBuf[5]= trigger count or data
            // txdBuf[6]= crc16
            // txdBuf[7]= crc16
            ////      DisableIntU1TX;
            ////      DisableIntU1RX;
            //
            attempts = 0;
            //
            sendText(txdBuf, 2, 10);
            thread_wait(10);

            LED_On(LED_D6);
            //            while (attempts < 1)
//            {
                attempts++;
                numBytes = 0;
                //           writebufferLen = strlen((char *)writeBuffer);
                //           UART2_Initialize();
                while (numBytes < writebufferLen) 
                {
                    int bytesToWrite = UART2_is_tx_ready();
                    numBytes += UART2_WriteBuffer(txdBuf + numBytes, bytesToWrite);

                }
        } // ends while(Qoutx!=Qinx)
        
                #define MY_BUFFER_SIZE 4

                char myBuffer[MY_BUFFER_SIZE];
                memset(&myBuffer[0], 0, MY_BUFFER_SIZE);
//                unsigned int numBytes;

                sendText("Waiting for response\n\r", 1, 0); // Debug message
//                thread_wait(10);
                
                numBytes = 0;
                do 
                {
                    if (UART2_TRANSFER_STATUS_RX_DATA_PRESENT & UART2_TransferStatusGet()) 
                    {
                        myBuffer[numBytes++] = UART2_Read();
                        sendText("Read byte\n\r", 1, 0); // Debug message
                        thread_wait(20);
                        uint8_t rslt = formatHex(&myBuffer[numBytes], rsltBuffer, 1);
                        
                        sendText(rsltBuffer, 1, 0);
                        thread_wait(20);
//                        
                    }

//                    sendText(myBuffer, 2, 6);
//                    sendCRLF();
                    
//                    thread_wait(30);
                    context_switch();
                    // Do something else...

                } while (numBytes < MY_BUFFER_SIZE);
                //        initUart1();
                //

                sendText("Done reading bytes\n\r", 1, 0); // Debug message
                sendText(myBuffer, 2, 10);
                thread_wait(10);
                PnlTxQoutx = PnlTxQinx;


                //        // clear rx fifo buffer
                ////        while (DataRdyUART1())
                //        {
                ////          i = ReadUART1();
                //        }
                //        txIndex = 0;
                //        rxIndex = 0;
                ////        rxdBuf[0] = 0;
                ////        rxdBuf[1] = 0;
                //        ++attempts;
                //        /* set timer for panel response */
                //        starttime = timers_milliseconds_get();
                ////        U1STAbits.UTXEN = 1; // start transmission
                //        //while (txIndex < txdBuf[0])
                ////        while (IEC0bits.U1TXIE != 0)
                ////        {
                ////          // wait for transmit complete
                ////          context_switch();
                ////        }
                //
                //        if (ptype == type_streamack)
                //        {
                //          // stream acks get no response from panel
                //          // speed up - short out the pacing timer while streaming
                //          datachecktime = timers_milliseconds_get() - 500;
                //          break;
                //        }
                //        datachecktime = timers_milliseconds_get();
                //
                //        // wait for a response for the panel
                //        // expecting 4 byte ack packet
                //        while (rxIndex < 4)
                //        {
                //          if (timers_milliseconds_elapsed(starttime) > 150)
                //          {
                //            ++timeout;
                //            ++retries;
                //            break; // retry if time is up
                //          }
                //          context_switch();
                //          continue;
                //        }
                //
                //        if (!(got_an_ack()))
                //        {
                //          // error if no ack
                //          ++errors;
                //          ++retries;
                //          //context_switch();
                //
                //          thread_wait(20); // short pause
                //          continue;        // retry
                //        }
                //        // success
                //        break;
                //          context_switch();

//            } // ends while
            //
            LED_Off(LED_D6);
            //            MessageSent = 0;

            //      releasePanelSerialChannel();
            //
            //      if (attempts > 5)
            //      {
            //        ++failcount;
            //      }
            //      else
            //      {
            //        ++passcount;
            //      }
            //
            //      if (++PnlTxQoutx >= RF_TX_QSIZE)
            //      {
            //        PnlTxQoutx = 0;
            //      }
            //
            //      thread_wait(25); // short pause
            //      //datachecktime = timers_milliseconds_get();
            //      continue;
        

        context_switch();
    }
}

/**
 *
 *  Calculate the crc over "data" with length len
 *
 * crc is calculated using the same polynomial
 * is that use by the Enterprise downloader
 *
 */
uint16_t calcCrc(uint8_t *data, uint16_t len) 
{
    uint16_t CRC, x;
    uint8_t temp, i;

    CRC = 0;
    for (x = 0; x < len; x++) 
    {
        temp = data[x];
        for (i = 0; i < 8; i++) 
        {
            temp ^= (CRC & 1);
            CRC >>= 1;
            if (temp & 1) 
            {
                CRC ^= 0xA001;
            }
            temp >>= 1;
        }
    }
    return CRC;
}
