/***************************************************************************
   file pnserial.c
    G Kackman
****************************************************************************/


#include "stddef.h"
#include "queue.h"
#include "rftrx.h"
#include "threads.h"
#include "ulpktdef.h"
#include "dlpktdef.h"
#include "statusmanager.h"
#include "sys_variables.h"
#include "dfformat.h"
//#include "voice.h"
#if 0 //KGS removed audio.c
#include "audio.h"

#endif
#include "uart.h"
#include "uart1.h"
#include "xprintf.h"
#include "pnserial.h"

#undef _ISR
#define _ISR __attribute__((interrupt, auto_psv))
#define TXNPKTS 2          // number of packets transmitted key press (cept emg)

#define LOOPBACKTESTMODE 0 // normal operation
//#define LOOPBACKTESTMODE 1  // connect tx to rx, send/receive simulated downlink packets
//#define LOOPBACKTESTMODE 2  // connect tx to rx, send/receive simulated uplink packets

#define RF_TX_QSIZE 10

#define TXPULSE 122               // in us
#define ONEB  3*TXPULSE           // one bit (366us)
#define ZEROB 2*TXPULSE           // zero bit (244us)
#define ST80  4*TXPULSE           // start bit in 80 bit format (488us)
#define TOL80  61                 // +/- bit tolerance, 80 bit format
#define TOL80S 43                 // +/- start bit tolerance, 80 bit format

#define N2000 2000*(FCPU/1000000) // 2000us
#define N2122 2122*(FCPU/1000000) // 2122us
#define N122   122*(FCPU/1000000) //  122us
#define N244   244*(FCPU/1000000) //  244us
#define N366   366*(FCPU/1000000) //  366us
//#define DLINK_TIMEOUT_SEC 8*60

//static uint16_t dlink_time;
uint16_t starttime, datachecktime;

//#define SUPERVISORY_TIMEOUT_SEC 64*60

//static uint16_t super_time;

#define STREAM_TIMEOUT_SEC 60

uint8_t got_an_ack( void );

extern uint16_t calcCrc( uint8_t* data, uint16_t len );

static uint16_t stream_time;

//uint8_t enroll_seq;
//int16_t enroll_timeout;

//uint8_t ptcount,atcount;
//uint8_t strerror0, strerror1, strerror2, strerror3, strerror4, strerror5;
//uint8_t cserror;

//static struct message_Q rf_tx_message_Q;         //container information... number of messages in que.
//static struct Message_Data rf_tx_Q[RF_TX_QSIZE]; //declare message storage array
extern struct NVDATA nvData;

extern struct MFGDATA mfgData;

extern struct VDATA vData;

extern enum NVBACKUPSTATE nvBackupStatus;
extern void voice_phrase_put( struct PHRASE_DATA *phraseN );
//extern void audio_keypadbeep( void );
//void send_enrollment_request_packet( void );
uint8_t rf_tx_Qinx;  // input index
uint8_t rf_tx_Qoutx; // output index
//uint8_t get_panel_firmware_ver( void );
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

//union RandomVar
//{
//    uint32_t rval;
///    uint8_t rbyte[4];
//} randomVar;

//uint32_t rndm( uint32_t x );

//uint8_t startbit; // fixme sb a bit flag

//extern uint8_t tsbuf[];

//extern uint8_t rfbuf[];

//unsigned int setOc1; // output compare value
//void set_rf_dlink_ok( void );
//void sendPacket( void );
//extern void calcCrc16( void );
//extern uint8_t checkCrc16( void );
//extern void calcCrc8( void );
//extern uint8_t checkCrc8( void );
//extern void advanceTxTrig( void );
//extern void encrypt();
//void process80Bit( void );
//void initMax1473( void );
void pn_trx_thread( void );
void sayOkN( uint8_t N );
void sayNumber( uint8_t num );

// "Anyone who considers arithmetical methods of producing random
//  digits is, of course, in a state of sin." John Von Neumann 1951
//
// occasionally we need some "random" bits
// simple fast linear congruential pseudo-random number generator
// (from Donald Knuth & H Lewis I thinK)

//uint32_t rndm( uint32_t x )
//{
//    uint32_t y;
//    y = (1664525L * x + 1013904223L);
//    return (y);
//}


// call this function to send uplink packets to alarm panel
// each packet carries TWTTS id and one byte of data
void rf_tx_message_put( struct Message_Data *new_msg )
{
  rf_tx_UQ[rf_tx_Qinx] = new_msg->message_info.uplink_pkt;

  if (++rf_tx_Qinx >= RF_TX_QSIZE)
  {
    rf_tx_Qinx = 0;
  }
}
#if 0

void set_rf_dlink_ok( void )
{
  extern struct VDATA vData;
  vData.rf_dlink_ok = 1;
  dlink_time = timers_seconds_get();
}

#endif

void sayNumber( uint8_t num )
{

#if 0

  extern void send_token_to_audio_thread(uint8_t token);
  extern void audio_keypadbeep(void);
  struct Message_Data msgx;
  audio_keypadbeep();
  send_token_to_audio_thread(num);
  msgx.tag = msg_say_audio_token;
  msgx.message_info.say_audio_token.token = num + 1;
  audio_message_put(&msgx);

#endif

}
/*
void send_battery_report( void )
{

    struct Message_Data event_msg;


    // send a low batt or batt ok report to the panel
    // note - these are send as "keypress's"
    event_msg.message_info.uplink_pkt.ulink_pkt_type = type_cmd;

    if (vData.battery_trouble == 1)
    {
        event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = TsLowBattery;
    }
    else
    {

        event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = TsBatteryOk;
    }
    rf_tx_message_put(&event_msg);
}
*/
/*
void send_supervisory_report( void )
{
    // send a supervisory uplink packet
    struct Message_Data event_msg;
    event_msg.message_info.uplink_pkt.ulink_pkt_type = type_keypress;
    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = SupervisoryPkt;
    rf_tx_message_put(&event_msg);
    super_time = timers_seconds_get();
}
*/

/******************************************************************
* returns panel firware rev as hex char major_rev.minor_rev
* ie 1.0 == 0x10
* panel_firmware_ver = 0x00 if never learned into a panel
*                    = 0x13 if panel does not send a firmware version
* v1.4 is lowest Simon XT version that is actually sends firmware version
********************************************************************/
uint8_t get_panel_firmware_ver( void )
{
//  return (nvData.panel_firmware_ver);
}

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

void pn_trx_thread( void )
{
  extern uint8_t txIndex;
  extern uint8_t txdBuf [];
  extern uint8_t rxIndex;
  extern uint8_t rxdBuf [];

  uint8_t i;
  uint8_t ptype;
  //       uint8_t txdBuf[10];
  //       uint8_t rxdbuf[10];
  uint16_t tmp;

  extern void context_switch(void);

  for (; ; ) // forever loop
  {

    //set_rf_dlink_ok();


    // tx messages are usually key code data
    // for each relevant key press uplink packets
    // are sent to the alarm panel with the
    // corresponding key code
    //
    // byte0 number of bytes to follow = 6
    // byte1 packet type (key or cmd)
    // byte2 key/command code
    // byte3 trig val or data
    // byte4 trig val or data
    // byte5 crcl
    // byte6 crch
    //

    while (rf_tx_Qoutx != rf_tx_Qinx)
    {
      waitForPanelSerialChannel();
//      DisableIntU1TX;
//      DisableIntU1RX;

      // next uplink data code to tx buffer
      txdBuf[0] = 7; // # of bytes to follow
      txdBuf[1] = PTS_KEYCMD;
      ptype = rf_tx_UQ[rf_tx_Qoutx].ulink_pkt_type;
      ptype &= 0x0f;
      txdBuf[2] = ptype;

      switch (ptype)
      {
        case type_keypress:
        case type_cmd:
          txdBuf[3] = rf_tx_UQ[rf_tx_Qoutx].uplink_pkts.key_value.keycode;
          txdBuf[4] = 0; // was tx trigger count
          txdBuf[5] = 0; // was tx trigger count
          break;

        default:
        case type_streamack:
          txdBuf[3] = rf_tx_UQ[rf_tx_Qoutx].uplink_pkts.generic_uplink.ulbyte0;
          txdBuf[4] = rf_tx_UQ[rf_tx_Qoutx].uplink_pkts.generic_uplink.ulbyte1;
          txdBuf[5] = rf_tx_UQ[rf_tx_Qoutx].uplink_pkts.generic_uplink.ulbyte2;
          break;
      }
      tmp = calcCrc(txdBuf, 6);        //crc over 6 bytes
      txdBuf[6] = (uint8_t)tmp;        //save the msb ofthe crc
      txdBuf[7] = (uint8_t)(tmp >> 8); //save lsb of crc

      // packet to send
      // txdBuf[0]= # of following bytes = 7
      // txdBuf[1]= PTS_KEYCMD
      // txdBuf[2]= ptype (type keypress or cmd or ...)
      // txdBuf[3]= key or cmd or data
      // txdBuf[4]= trigger count or data
      // txdBuf[5]= trigger count or data
      // txdBuf[6]= crc16
      // txdBuf[7]= crc16
//      DisableIntU1TX;
//      DisableIntU1RX;

      attempts = 0;

      while (attempts <= 5)
      {
        initUart1();

        // clear rx fifo buffer
//        while (DataRdyUART1())
        {
//          i = ReadUART1();
        }
        txIndex = 0;
        rxIndex = 0;
//        rxdBuf[0] = 0;
//        rxdBuf[1] = 0;
        ++attempts;
        /* set timer for panel response */
        starttime = timers_milliseconds_get();
//        U1STAbits.UTXEN = 1; // start transmission
        //while (txIndex < txdBuf[0])
//        while (IEC0bits.U1TXIE != 0)
//        {
//          // wait for transmit complete
//          context_switch();
//        }

        if (ptype == type_streamack)
        {
          // stream acks get no response from panel
          // speed up - short out the pacing timer while streaming
          datachecktime = timers_milliseconds_get() - 500;
          break;
        }
        datachecktime = timers_milliseconds_get();

        // wait for a response for the panel
        // expecting 4 byte ack packet
        while (rxIndex < 4)
        {
          if (timers_milliseconds_elapsed(starttime) > 150)
          {
            ++timeout;
            ++retries;
            break; // retry if time is up
          }
          context_switch();
          continue;
        }

        if (!(got_an_ack()))
        {
          // error if no ack
          ++errors;
          ++retries;
          //context_switch();

          thread_wait(20); // short pause
          continue;        // retry
        }
        // success
        break;
      } // ends while

      releasePanelSerialChannel();

      if (attempts > 5)
      {
        ++failcount;
      }
      else
      {
        ++passcount;
      }

      if (++rf_tx_Qoutx >= RF_TX_QSIZE)
      {
        rf_tx_Qoutx = 0;
      }

      thread_wait(25); // short pause
      //datachecktime = timers_milliseconds_get();
      continue;
    } // ends while(Qoutx!=Qinx)

    // periodically poll panel for data
    if (timers_milliseconds_elapsed(datachecktime) > 100)
    {
      waitForPanelSerialChannel();
//      DisableIntU1TX;
//      DisableIntU1RX;

      // packet to send
      // txdBuf[0]= # of following bytes = 3
      // txdBuf[1]= PTS_DATAQUERY
      // txdBuf[3]= crc16
      // txdBuf[4]= crc16

      txdBuf[0] = 3; // # of bytes to follow
      txdBuf[1] = PTS_DATAQUERY;

      attempts = 0;

      while (attempts <= 3)
      {
        tmp = calcCrc(txdBuf, 2);        //crc over 2 bytes
        txdBuf[2] = (uint8_t)tmp;        //save the msb ofthe crc
        txdBuf[3] = (uint8_t)(tmp >> 8); //save lsb of crc

        initUart1();

        /* reload interval timer */
        datachecktime = timers_milliseconds_get();

        // clear rx fifo buffer
//        while (DataRdyUART1())
        {

//          i = ReadUART1();
        }
        txIndex = 0;
        rxIndex = 0;
//        rxdBuf[0] = 0;
//        rxdBuf[1] = 0;
        ++attempts;
        /* set timer for panel response */
        starttime = timers_milliseconds_get();
//        U1STAbits.UTXEN = 1; // start

//        while ((rxIndex == 0) || (rxdBuf[0] == 0) || (rxIndex != (rxdBuf[0] + 1)))
//        {
//          if (timers_milliseconds_elapsed(starttime) > 150)
//          {
//            ++timeout1;
//            ++retries1;
//            break; // retry if time is up
//          }
//          context_switch();
//        }
        // rxIndex>0 &&
        // rxdBuf[0]>0 &&
        // rxIndex=rxdBuf[0]+1

//        if (calcCrc(rxdBuf, rxIndex) != 0)
//        {
//          // crc error
//          txdBuf[1] = PTS_RESEND;
//          ++crcerrors1;
//          ++retries1;
//          thread_wait(20); // short pause
//          continue;        // retry
//        }

//        if ((rxdBuf[1] == DL_ACK) && (rxIndex == 4))
//        {
//          break; // success
//        }

//        if ((rxdBuf[1] == PTS_DATA) && (rxIndex == 9))
        {
          // success
          // send data "upstairs"
          struct Message_Data rf_msg;
          rf_msg.tag = msg_rf_rx;
          rf_msg.message_info.rf_packet_data.rf_message.type = (enum DlPktType)(rxdBuf[2]);
          rf_msg.message_info.rf_packet_data.rf_message.rf_message_data.generic_pkt.byte1 = rxdBuf[3];
          rf_msg.message_info.rf_packet_data.rf_message.rf_message_data.generic_pkt.byte2 = rxdBuf[4];
          rf_msg.message_info.rf_packet_data.rf_message.rf_message_data.generic_pkt.byte3 = rxdBuf[5];
          rf_msg.message_info.rf_packet_data.rf_message.rf_message_data.generic_pkt.byte4 = rxdBuf[6];
          status_message_put(&rf_msg);
          break;
        }

        if (rxdBuf[1] == PTS_STREAM)
        {
          if (rxIndex == 140)
          {
            // success
            process_stream_packet();
            break;
          }
        }
        // packet not acceptable
        txdBuf[1] = PTS_RESEND;
        ++retries1;
        thread_wait(20); // short pause
        continue;        // retry
      }                  // ends while

      releasePanelSerialChannel();

      if (attempts > 3)
      {
        ++failcount;
      }
      else
      {
        ++passcount;
      }
    }
    // some misc time limits
    // may not belong here
    // but has to go somewhere

    if (block_dl_in_progress != 0)
    {
      // time limit for streaming
      if (timers_seconds_elapsed(stream_time) > STREAM_TIMEOUT_SEC)
      {

        block_dl_in_progress = 0;
      }
    }
    /*
    dormant
            if (timers_seconds_elapsed(dlink_time) > DLINK_TIMEOUT_SEC)
            {
                extern struct VDATA vData;
                vData.rf_dlink_ok = 0; // flag rf downlink failure
            }
    */
    /*
    dormant
            if (timers_seconds_elapsed(super_time) > SUPERVISORY_TIMEOUT_SEC)
            {
                // send a supervisory uplink
                //////send_supervisory_report();
                super_time = timers_seconds_get();
            }
    */
    context_switch();
  } // ends forever
}   // ends tx thread

void pn_trx_thread_init( void )
{
  static THREAD pn_trx_thread_struct;

  //declare stack for opn_trx thread  TBD determine needed size
  static uint8_t pn_trx_thread_stack[1000/*550*/];

  thread_create(pn_trx_thread, &pn_trx_thread_struct, pn_trx_thread_stack, sizeof(pn_trx_thread_stack));
}

/**
 *
 *  check if specified packet has been received
 *
 *  returns: 0 => specified packet not received
 *           1 => specified packet received
 *
 * 			 200 => lenght error
 *           201 => crc error
 */
uint8_t receivedSpecifiedPkt( uint8_t packet ) //TBD Gerry check for accuracy
{

  extern uint8_t rxIndex;
  extern uint8_t rxdBuf [];

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
  extern uint8_t rxdBuf [];

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

/**
 *  write
 */
uint16_t writePanelData( struct PANELDATA *panelData )
{
  extern uint8_t txIndex;
  extern uint8_t txdBuf [];
  extern uint8_t rxIndex;
  extern uint8_t rxdBuf [];

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
  waitForPanelSerialChannel();
//  DisableIntU1TX;
//  DisableIntU1RX;
  // here to send a block of data
  txdBuf[0] = len + 3;         // # of bytes to follow
  txdBuf[1] = PTS_BLOCK_WRITE; // code

  txdBuf[2] = panelData->token & 0xff;
  txdBuf[3] = panelData->token >> 8;
  txdBuf[4] = panelData->subIndex;

  copyN(panelData->data.array, &txdBuf[2 + sizeof(panelData->token) + sizeof(panelData->subIndex)],
      panelData->inputDataLen);
  tmp = calcCrc(txdBuf, len + 2);        //crc over n+2 bytes
  txdBuf[len + 2] = (uint8_t)tmp;        //save the msb ofthe crc
  txdBuf[len + 3] = (uint8_t)(tmp >> 8); //save lsb of crc

  attempts = 0;

  while (attempts <= 2)
  {
    initUart1();
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
    if (got_an_ack())
    {
      // success
      break;
    }
  }
  releasePanelSerialChannel();

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
  extern uint8_t rxdBuf [];

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
  waitForPanelSerialChannel();
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
  copyN(panelData->data.array, &txdBuf[9], panelData->inputDataLen); //input data
  //CRC over length of packet:  calculation does includes length byte (txdBuf[0]) itself but does not include crc bytes
  //-1 in calcCRC() below = +1 byte length(txdBuf[0] - 2 bytes for crc byte length
  tmp = calcCrc(txdBuf, txdBuf[0] - 1); //calculate crc over length of packet(txdBuf[0] excluding the 2 crc bytes
  txdBuf[9 + panelData->inputDataLen] = (uint8_t)tmp;        //save the msb of the crc
  txdBuf[10 + panelData->inputDataLen] = (uint8_t)(tmp >> 8); //save lsb of crc

  attempts = 0;

  while (attempts <= 2)
  {
    initUart1();
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
        copyN(&rxdBuf[10], panelData->data.array, panelData->inputDataLen);
        panelData->data.array[panelData->inputDataLen] = 0;
        panelData->dataInfoLen = rxdBuf[9];
                                 /*rxdBuf[0] - sizeof(panelData->token) - sizeof(panelData->subIndex) 
                                           - sizeof(panelData->dataType) - sizeof(panelData->inputDataLen)
                                           - sizeof(panelData->dataInfoLen)
                                           - panelData->inputDataLen - 4;*/ //-4 => 1 byte status + 2 byte crc length +  1 byte for PTS_BLOCK_READ message
        copyN(&rxdBuf[10] + panelData->inputDataLen, panelData->dataInfo, panelData->dataInfoLen);
        releasePanelSerialChannel();
        return rxdBuf[2]; //return the status of the packet read
      }
      context_switch();
    }
  }
  releasePanelSerialChannel();
  return eUIItemError;
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


/*
 *
 * sends beep packet to panel
 * keypad beeps are generated by the panel's audio system
 *
 */
void audio_keypadbeep( void )
{
  struct Message_Data event_msg;
  event_msg.message_info.uplink_pkt.ulink_pkt_type = type_cmd;
  event_msg.message_info.uplink_pkt.uplink_pkts.cmd.cmdcode = KeyBeep;
  rf_tx_message_put(&event_msg);
}


// rxdBuf[0] =  packet length (#bytes to follow)
// rxdBuf[1] =  PTS_STREAM
// rxdBuf[2] =  tsbuf[0]
// rxdBuf[3] =  tsbuf[1]
// rxdBuf[4] =  tsbuf[2]
// rxdBuf[5] =  tsbuf[3]
// rxdBuf[6] =  tsbuf[4]
// rxdBuf[7] =  tsbuf[5]
// rxdBuf[8] =  tsbuf[6]
// rxdBuf[9] =  tsbuf[7]
// rxdBuf[a] =  tsbuf[8]
// rxdBuf[b] =  tsbuf[9]
// rxdBuf[c] =  serial crc
// rxdBuf[d] =  serial crc

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

    stream_time = timers_seconds_get(); // set a time limit for streaming
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
    rf_tx_message_put(&event_msg);

    stream_time = timers_seconds_get(); // set a time limit for streaming
  }
  return;
}
