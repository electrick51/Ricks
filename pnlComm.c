/*
 * File:   pnlComm.c
 * Author: Rschneew
 *
 * Created on March 18, 2018, 10:34 AM
 */

#include "xc.h"
#include "queue.h"
#include "debugMessages.h"
#include "TouchscreenFiles/threads.h"
#include "TouchscreenFiles/queue.h"
#include "leds.h"
#include "mcc_generated_files/uart1.h"
#include "TouchscreenFiles/sys_variables.h"
#include "pnlComm.h"
#include "TouchscreenFiles/pnserial.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "sys_variables.h"

#include "statusmanager.h"




#define RF_TX_QSIZE 10
#define RX_BUFFER_SIZE 10
struct Message_Data rf_msg;
static struct Message_Data debugMsg;
uint8_t readAccumulator[30];
uint8_t index = 0;
extern struct VDATA vData;         //volatile system variables

extern uint8_t MessageSent; // Test variable
static uint8_t receiveBusy;

static void pnlComm_thread(void);
static const char *p_token_to_text[];

void waitForPanelSerialChannel( void );
void releasePanelSerialChannel( void );
void process_stream_packet( void );

struct Uplink_Pkts rf_tx_UQ[RF_TX_QSIZE];

uint8_t stream_buf[STREAM_BLOCK_SZ + 4];

// stream buffer
//  bytes 0-131      132 data bytes
//  byte  132        csum verified=1
//  byte  133        stream id code
//  byte  134        block seq number in (0, N-1)
//  byte  135        N blocks in stream
uint8_t dr_dl_trigger; // trigger val for directed packets
//uint8_t bc_dl_trigger;        // trigger val for broadcast packets
uint8_t bcpkt_sav[5];         // save broadcast packet
uint8_t block_dl_in_progress; // flag
uint8_t stream_id_code;
uint8_t block_seq_num;
uint8_t number_of_blocks;
//uint8_t pkt_seq_num;

extern uint16_t pktCtrRangeTest,pktCtrDr,pktCtrBc,pktCtrLearn,pktCtrCrcErr;

extern uint16_t pktCtrRaw80b,pktCtrValid80b;

#define status_QSIZE 10
static struct message_Q pnlComm_message_Q; //container information... number of messages in que.
static struct Message_Data status_Q[status_QSIZE]; //declare message storage array
uint8_t PnlTxQinx = 0; // input index
uint8_t PnlTxQoutx = 0; // output index
static uint8_t rxdBuf[RX_BUFFER_SIZE];

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

uint8_t txIndex;
uint8_t txdBuf [16];
uint8_t rxIndex;
//uint8_t rxdBuf [16];
    
uint8_t receivedSpecifiedPkt( uint8_t packet ); //TBD Gerry check for accuracy


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
//    sendText("Received a Q message\n\r", 1, 0);
//    sendText(new_msg->message_info.uplink_pkt.uplink_pkts.key_value.keycode, 2, 1);
//    thread_wait(30);
    
    if (++PnlTxQinx >= RF_TX_QSIZE) 
    {
        PnlTxQinx = 0;
//        PnlTxQoutx = 0;
//        PnlTxUQ[PnlTxQinx] = new_msg->message_info.uplink_pkt;
//        sendText("Set PnlTxQinx to zero\n\r", 1, 0);
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
    uint8_t numbytes = 20;
    uint8_t buffer[8] = {0x01, 0x22, 0x33, 0x26, 0x01, 0x22, 0x33, 0x26};
    //    UART1_Initialize();
    //    UART1_WriteBuffer(buffer , sizeof(buffer) );



    for (;;) 
    {
        LED_On(LED_D5);
        LoopTimer3++;
        if (LoopTimer3 > 16000) 
        {

            //          UART1_WriteBuffer(buffer , sizeof(buffer) );
            //          UART1_Write( 0x34);
            LoopTimer3 = 0;
        }


        //*****************************************************************
        //  Gerry's Tx code from Pnserial.c

        
        
        while (PnlTxQoutx != PnlTxQinx) 
        {

            uint8_t dbBuffer[4] = {PnlTxQoutx, PnlTxQinx, 0xFF, 0xFF};
            
//            sendText(dbBuffer, 2, 4);
            
//            sendText("Got a packet to send\n\r", 1, 0); // Debug message
//            thread_wait(10);

            buildDebugMessage(&debugMsg, "Got a packet to send\n\r", 1, 0, "PC");
            debugMsg.tag = msg_debug_message;
//            DebugMessage_message_put(&debugMsg);
            context_switch();
               //             sendText("Sending One key\n\r", 1, 0);
            thread_wait(30);
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
//            sendText(txdBuf, 2, 8);
//            thread_wait(10);

            LED_On(LED_D6);
            //            while (attempts < 1)
            {
                attempts++;
                int numBytes = 0;
                //           writebufferLen = strlen((char *)writeBuffer);
                //           UART2_Initialize();
                while (numBytes < writebufferLen) 
                {
                    int bytesToWrite = UART2_is_tx_ready();
                    numBytes += UART2_WriteBuffer(txdBuf + numBytes, bytesToWrite);

                }

                

                
                memset(&rxdBuf[0], 0, RX_BUFFER_SIZE);
//                unsigned int numBytes;

//                sendText("Waiting for response\n\r", 1, 0); // Debug message
//                thread_wait(10);
                
                numBytes = 0;
//                receiveBusy = 1;
////                do 
////                {
//                   while (UART2_TRANSFER_STATUS_RX_DATA_PRESENT & UART2_TransferStatusGet()) // if
//                    {
//                        rxdBuf[numBytes++] = UART2_Read();
////                        sendText("Read byte\n\r", 1, 0); // Debug message
////                        thread_wait(20);
////                        uint8_t rslt = formatHex(&rxdBuf[numBytes -1], rsltBuffer, 1);
////                        
////                        sendText(rsltBuffer, 1, 0);
////                        thread_wait(20);
////                        
//                    }
//
//                receiveBusy = 0;
//                sendText("Done reading bytes\n\r", 1, 0); // Debug message
//                sendText(rxdBuf, 2, numBytes);
//                thread_wait(20);
//                PnlTxQoutx = PnlTxQinx;

                if (++PnlTxQoutx >= RF_TX_QSIZE)
                {
                  PnlTxQoutx = 0;
                }

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

            } // ends while
            //
            LED_Off(LED_D6);
            MessageSent = 0;

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
        } // ends while(Qoutx!=Qinx)

        LED_Off(LED_D5);
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


// ping = 03 72 80 D5

void send100msPing(void)    // Called from 100 ms interrupt
{
    uint8_t pingBuff[4] = {0x03, 0x72, 0x80, 0xD5};
    int writeLen = 4;
    uint8_t x;
    uint8_t rsltBuffer[4];
    
    if(receiveBusy == 0)
    {
            LED_On(LED_D6);

            int numBytes = 0;

            numBytes = UART2_WriteBuffer(pingBuff, writeLen);


            LED_Off(LED_D6);
            
            
             memset(&rxdBuf[0], 0, RX_BUFFER_SIZE);

             numBytes = 0;
  
            while (UART2_TRANSFER_STATUS_RX_DATA_PRESENT & UART2_TransferStatusGet()) 
            {
                rxdBuf[numBytes++] = UART2_Read();

            }

             if(numBytes == 1)
             {
                 readAccumulator[index++] = rxdBuf[0];
                 
             }
             else
             {
                 if(index > 4)
                 {
                    for(x = 0; x < index; x++, numBytes++)
                    {
                        rxdBuf[numBytes] = readAccumulator[x];
                    }
                 }
                 index = 0;
             }
             
             int bytesRecvd = numBytes;
             if(numBytes > 0)
             {
                 buildDebugMessage(&debugMsg, "Done reading bytes\n\r", 1, 0, "PC");
                 debugMsg.tag = msg_debug_message;
//                 DebugMessage_message_put(&debugMsg);

                 buildDebugMessage(&debugMsg, rxdBuf, 2, numBytes, "PC");
                 debugMsg.tag = msg_debug_message;
//                 DebugMessage_message_put(&debugMsg);

                rf_msg.tag = msg_rf_rx;
                rf_msg.message_info.rf_packet_data.rf_message.type = (enum DlPktType)(rxdBuf[2]);
                rf_msg.message_info.rf_packet_data.rf_message.rf_message_data.generic_pkt.byte1 = rxdBuf[3];
                rf_msg.message_info.rf_packet_data.rf_message.rf_message_data.generic_pkt.byte2 = rxdBuf[4];
                rf_msg.message_info.rf_packet_data.rf_message.rf_message_data.generic_pkt.byte3 = rxdBuf[5];
                rf_msg.message_info.rf_packet_data.rf_message.rf_message_data.generic_pkt.byte4 = rxdBuf[6];
                status_message_put(&rf_msg);
             }
                
//           context_switch();
          
    }
        
   
    
}

void checkForPanelMessages(void)    // Called from 50 ms interrupt
{
    static uint8_t checkCounter;
    
    memset(&rxdBuf[0], 0, RX_BUFFER_SIZE);
    unsigned int numBytes;
    unsigned char rsltBuffer[10];

    memset(&rsltBuffer[0], 0, 10);
    
    
    if(++checkCounter > 2)
    {
        
//    
//                

//    sendText("Waiting for response\n\r", 1, 0); // Debug message
//                thread_wait(10);
                
    numBytes = 0;
//    do 
//    {
        if (UART2_TRANSFER_STATUS_RX_DATA_PRESENT & UART2_TransferStatusGet()) 
        {
            if(!receiveBusy)
            {
                rxdBuf[numBytes++] = UART2_Read();
            }
            

        }

        
                    // Do something else...

//    } while (numBytes < RX_BUFFER_SIZE);
        checkCounter = 0;
//        context_switch();
    }
}


// *************************************************************  Read/Write Panel Data ******************************************************

/**
 *  write
 */
uint16_t writePanelData( struct PANELDATA *panelData )
{
  extern uint8_t txIndex;
  extern uint8_t txdBuf [];
  extern uint8_t rxIndex;
//  extern uint8_t rxdBuf [];

  uint8_t i, len;
  uint16_t tmp;

  len = sizeof(panelData->token) + sizeof(panelData->subIndex) + panelData->inputDataLen;

  //    nbytes
  //    PTS_BLOCK_WRITE code
  //    data
  //    ....
  //    ....
  //    crc
  //    crc
  if ((len == 0) || (len > 256 - 4))
  {
    return -1;
  }
  // wait for the talking stick
//  waitForPanelSerialChannel();
//  DisableIntU1TX;
//  DisableIntU1RX;
  // here to send a block of data
  txdBuf[0] = len + 3;         // # of bytes to follow
  txdBuf[1] = PTS_BLOCK_WRITE; // code

  txdBuf[2] = panelData->token & 0xff;
  txdBuf[3] = panelData->token >> 8;
  txdBuf[4] = panelData->subIndex;

//  copyN(panelData->data.array, &txdBuf[2 + sizeof(panelData->token) + sizeof(panelData->subIndex)], panelData->inputDataLen);
  tmp = calcCrc(txdBuf, len + 2);        //crc over n+2 bytes
  txdBuf[len + 2] = (uint8_t)tmp;        //save the msb ofthe crc
  txdBuf[len + 3] = (uint8_t)(tmp >> 8); //save lsb of crc

  attempts = 0;

  while (attempts <= 2)
  {
//    initUart1();
    /* reload interval timer */
    datachecktime = timers_milliseconds_get();

    // clear rx fifo buffer
//    while (DataRdyUART1())
    {

//      i = ReadUART1();
    }
    txIndex = 0;
    rxIndex = 0;
    ++attempts;
    /* set timer for panel response */
    starttime = timers_milliseconds_get();
//    U1STAbits.UTXEN = 1; // start transmission

    while (rxIndex < 4)
    {
      if (timers_milliseconds_elapsed(starttime) > 500)
      {
        ++timeout1;
        ++retries1;
        break; // retry if time is up
      }
      context_switch();
    }

    // check for an ack
//    if (got_an_ack())
//    {
//      // success
//      break;
//    }
  }
//  releasePanelSerialChannel();

  // success

  if (attempts > 2)
  {
    ++failcount;
    return -1;
  }
  else
  {
    ++passcount;
    return 0;
  }
}

/**
 *  read
 */
enum RESOURCESTATUS readPanelData( struct PANELDATA *panelData )
{
  extern uint8_t txIndex;
  extern uint8_t txdBuf [];
  extern uint8_t rxIndex;
//  extern uint8_t rxdBuf [];

  uint8_t i;
  uint16_t tmp;

  //return eUIItemError; //kgs remove

  //    4 bytes of data (token low and token high)
  //    PTS_BLOCK_READ code
  //    token low
  //    token high
  //    subIndex
  //    dataType low
  //    dataType high
  //    inputDateLen
  //    dataInfoLen
  //    inputData[] - inputDateLen bytes long
  //    crc
  //    crc
  // wait for the talking stick
//  waitForPanelSerialChannel();
//  DisableIntU1TX;
//  DisableIntU1RX;
  // here to send a block of data
  starttime = timers_milliseconds_get();

  if (timers_milliseconds_elapsed(starttime) < 100)
  {
  //  settling time
  }
  txdBuf[0] = 8 + panelData->inputDataLen + 2; //txdBuf[0] = length =  6(data bytes 1-6) + (array data[] length) + (2 crc bytes)
  txdBuf[1] = PTS_BLOCK_READ; //data byte 1          //Note: length of packet does not include the byte which contains the length of the packet (txdBuf[0])
  txdBuf[2] = panelData->token & 0xff;                               //data byte 2
  txdBuf[3] = panelData->token >> 8;                                 //data byte 3
  txdBuf[4] = panelData->subIndex;                                   //data byte 4
  txdBuf[5] = 0;                                                     //data byte 5 (unused always 0)
  txdBuf[6] = 0;                                                     //data byte 6 (unssed always 0)
  txdBuf[7] = panelData->inputDataLen; //array[]input data with length == panelData->inputDataLen;
  txdBuf[8] = 0; //dataInfoLen always 0
//  copyN(panelData->data.array, &txdBuf[9], panelData->inputDataLen); //input data
  //CRC over length of packet:  calculation does includes length byte (txdBuf[0]) itself but does not include crc bytes
  //-1 in calcCRC() below = +1 byte length(txdBuf[0] - 2 bytes for crc byte length
  tmp = calcCrc(txdBuf, txdBuf[0] - 1); //calculate crc over length of packet(txdBuf[0] excluding the 2 crc bytes
  txdBuf[9 + panelData->inputDataLen] = (uint8_t)tmp;        //save the msb of the crc
  txdBuf[10 + panelData->inputDataLen] = (uint8_t)(tmp >> 8); //save lsb of crc

  attempts = 0;

  while (attempts <= 2)
  {
//    initUart1();
    /* reload interval timer */
    datachecktime = timers_milliseconds_get();

    // clear rx fifo buffer
//    while (DataRdyUART1())
    {

//      i = ReadUART1();
    }
    txIndex = 0;
    rxIndex = 0;
    ++attempts;
    /* set timer for panel response */
    starttime = timers_milliseconds_get();
//    U1STAbits.UTXEN = 1; // start transmission

    while (1)
    {
      tmp = timers_milliseconds_elapsed(starttime);

      if (tmp > 1500) //wait no more than 1.5 second
      {
        break;        // retry if time is up
      }

      // check for data
      // rxdBuf[0] = length of received packet excluding length itself
      // rxdBuf[1] = packet block type should be: PTS_BLOCK_READ
      // rxdBuf[2] = status of the read (see enum RESOURCESTATUS)
      // rxdBuf[3 thru 4] = data item token
      // rxdBuf[5] = subIndex
      // rxdBuf[6 thru 7] = data type
      // rxdBuf[8] = input data len
      // rxdBuf[9 thru 9+rxdBuf[8] = input data
      // rxdBuf[9 thru rxdBuf[8]] - end = format string
      if (receivedSpecifiedPkt(PTS_BLOCK_READ) == RECEIVEDPACKET)
      {
        // success
        panelData->token = rxdBuf[3] | (rxdBuf[4] << 8);
        panelData->subIndex = rxdBuf[5];
        panelData->dataType = rxdBuf[6] | (rxdBuf[7] << 8);
        panelData->inputDataLen = rxdBuf[8];
//        copyN(&rxdBuf[10], panelData->data.array, panelData->inputDataLen);
        panelData->data.array[panelData->inputDataLen] = 0;
        panelData->dataInfoLen = rxdBuf[9];
                                 /*rxdBuf[0] - sizeof(panelData->token) - sizeof(panelData->subIndex) 
                                           - sizeof(panelData->dataType) - sizeof(panelData->inputDataLen)
                                           - sizeof(panelData->dataInfoLen)
                                           - panelData->inputDataLen - 4;*/ //-4 => 1 byte status + 2 byte crc length +  1 byte for PTS_BLOCK_READ message
//        copyN(&rxdBuf[10] + panelData->inputDataLen, panelData->dataInfo, panelData->dataInfoLen);
//        releasePanelSerialChannel();
        return rxdBuf[2]; //return the status of the packet read
      }
      context_switch();
    }
  }
//  releasePanelSerialChannel();
  return eUIItemError;
}


uint8_t receivedSpecifiedPkt( uint8_t packet ) //TBD Gerry check for accuracy
{

  extern uint8_t rxIndex;
//  extern uint8_t rxdBuf [];

  if (rxIndex)
  {
    if (rxdBuf[0] < 6) //must be at least: 1 byte packet type + 2 bytes token + 1 byte status + 2 bytes crc
    {
      return PACKETERROR;
    }

    if (rxdBuf[0] >= rxIndex)
    {
      return NOPACKETRX;
    }

    if (rxdBuf[1] != packet)
    {
      return UNEXPECTEDPACKET;
    }

    if (calcCrc(rxdBuf, rxdBuf[0] + 1))
    {
      return PACKETERROR;
    }
    return RECEIVEDPACKET;
  }
  return NOPACKETRX;
}

uint8_t got_an_ack( void )
{
  extern uint8_t rxIndex;
//  extern uint8_t rxdBuf [];

  if ((rxIndex == 4) &&    \
  (rxdBuf[0] == 3) &&      \
  (rxdBuf[1] == DL_ACK) && \
  (!(calcCrc(rxdBuf, 4)))) \
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*
*  test and set function for the panel serial port
*  wait until the serial channel is available then
*  sets the serial channel busy and returns
*  call to get possession of the panel serial channel
*/
void waitForPanelSerialChannel( void )
{
  while (vData.panel_serial_channel_busy)
  {
    context_switch();
  }
  vData.panel_serial_channel_busy = 1;
}
/*
*  call to release the panel serial chan
*/
void releasePanelSerialChannel( void )
{
  vData.panel_serial_channel_busy = 0;
}


void process_stream_packet( void )
{
  extern uint8_t rxdBuf [];
  //static uint8_t photo_id_code;
  uint8_t i;
  struct Message_Data event_msg;

  if (rxdBuf[2] == StartStreamBlockCode)
  {

    stream_id_code = rxdBuf[3];
    block_seq_num = rxdBuf[4];
    number_of_blocks = rxdBuf[5]; // total #of blocks in the stream

    for (i = 0; i < sizeof(stream_buf); ++i)
    {
      stream_buf[i] = 0;
    }

//    stream_time = timers_seconds_get(); // set a time limit for streaming
    //set_rf_dlink_ok();
    // incoming stream block pkt
    // 0     # bytes following
    // 1     PTS_STREAM
    // 2     block code
    // 3     stream id number     // type of stream (0=zone text, 1=logo, ...)
    // 4     block seq num
    // 5     # of blocks in stream
    // 6-137 132 data bytes
    // 138   checksum
    // 139   checksum

    for (i = 0; i < sizeof(stream_buf); ++i)
    {
      stream_buf[i] = rxdBuf[i + 6];
    }
    // tack on some stuff and end of buffer
    stream_buf[STREAM_BLOCK_SZ] = 1; // checksum verified
    stream_buf[STREAM_BLOCK_SZ + 1] = stream_id_code;
    stream_buf[STREAM_BLOCK_SZ + 2] = block_seq_num;
    stream_buf[STREAM_BLOCK_SZ + 3] = number_of_blocks;

    // process received stream block
    event_msg.tag = msg_stream_data;
    event_msg.message_info.stream_data.stream_id = stream_id_code;                     //stream's id
    event_msg.message_info.stream_data.pData = stream_buf;                             //pointer to stream data;
    event_msg.message_info.stream_data.block_number = stream_buf[STREAM_BLOCK_SZ + 2]; //current block number of stream
    event_msg.message_info.stream_data.status = stream_inprocess;

    switch (stream_id_code)
    {
      // photos are sent in a sequence of one block streams
      // a photo sequence number replaces
      // the block sequence number
      // the following code combines a sequence of
      // one block streams into a single stream photo stream
      case photographs:     // starts photo
      case photograph_next: // continues photo
        break;

      case photograph_end:
        event_msg.message_info.stream_data.status = stream_done;
        break;

      // non-photo streams come here
      default:
        if ((stream_buf[STREAM_BLOCK_SZ + 2] + 1) == stream_buf[STREAM_BLOCK_SZ + 3]) // check if stream is complete
        {
          event_msg.message_info.stream_data.status = stream_done;
        }
    }

    if (stream_id_code != 0xff) // 0xff is a test stream
    {
      buffer_stream(&event_msg.message_info.stream_data, event_msg.message_info.stream_data.stream_id);
    }

    // enque a stream ack for panel
    event_msg.message_info.uplink_pkt.ulink_pkt_type = type_streamack; // ack stream packet
    event_msg.message_info.uplink_pkt.uplink_pkts.stream_ack_uplink.stream_id = stream_id_code;
    event_msg.message_info.uplink_pkt.uplink_pkts.stream_ack_uplink.block_seq = block_seq_num;
    //event_msg.message_info.uplink_pkt.uplink_pkts.stream_ack_uplink.pkt_seq = 0; //not used
    pnlComm_put(&event_msg);

//    stream_time = timers_seconds_get(); // set a time limit for streaming
  }
  return;
}
