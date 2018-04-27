/**
 * Filename: pnserial.h
 *  Version: 1.0.0
 *
 *  header for pnserial.c.
 *
 */
#ifndef PNSERIAL_H
#define PNSERIAL_H

#include "exactint.h"
#include "dlpktdef.h"
#include "resources.h"

// front panel touchscreen serial uplink commands
#define PTS_DATAQUERY   0x72 // panel front screen asks any data for me?
#define PTS_KEYCMD      0x73 // panel front screen sends uplink packet with keys, cmds ...
#define PTS_BLOCK_WRITE 0x74 // panel front screen sends write uplink block data packet
#define PTS_BLOCK_READ  0x75 // panel front screen sends read uplink block data packet
#define PTS_RESEND      0x76 // panel front screen ask resend last packet
#define PTS_STREAM      0x77 // panel front screen

// front panel touchscreen serial downlink commands

#define PTS_DATA       0x72 // data packet to panel front screen

#define DL_ACK 0x7          // downloader protocol code

#define NOPACKETRX 0		//no packet received
#define RECEIVEDPACKET 1	//received expected packet
#define UNEXPECTEDPACKET 2  //wrong packet received
#define PACKETERROR 3		//=> packet received incorrectly

enum RESOURCESTATUS readPanelData(struct PANELDATA *panelData);
uint16_t writePanelData(struct PANELDATA *panelData);
void audio_keypadbeep( void );
void pn_trx_thread_init( void );
void waitForPanelSerialChannel( void );
void releasePanelSerialChannel( void );

#endif
