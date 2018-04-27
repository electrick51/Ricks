#ifndef RTCC_H
#define RTCC_H
#include "dlpktdef.h"
#include "display.h"
#include <string.h>

// macro
//#define mRTCCDec2Bin(Dec) (10*((uint8_t)(Dec)>>4)+((uint8_t)(Dec)&0x0f))  //convert packet bcd to decimal
//#define mRTCCBin2Dec(Bin) (((Bin/10)<<4)|(Bin%10))  //convert decimal to packet bcd

#if 0
typedef struct
{	/* date and time components in packed format -- compressed to send over 319.5 channel*/
   //uint16_t tm_sec:6; 
   uint16_t tm_min:6;
	uint16_t tm_hour:5;
	uint16_t tm_wday:3;
	uint16_t tm_mday:5;
	uint16_t tm_mon:4;
	uint16_t tm_year:5;              //good until 2009 + 32
}PACKED_TM;
#endif

// Union to access rtcc registers
typedef union tagRTCC 
{
	struct {
		uint8_t tm_sec;      //0-59 (packed bcd)
		uint8_t tm_min;      //0-59 (packed bcd)
		uint8_t tm_hour;     //0-23 (packed bcd)
		uint8_t tm_wday;     //0-6  week day
		uint8_t tm_mday;     //1-xx day of month (packed bcd)
		uint8_t tm_mon;      //month 1-12 (packed bcd)
		uint8_t tm_year;     //year 0-99 (packed bcd)
	};
	struct 
   {
		uint16_t prt00;
		uint16_t prt01;
		uint16_t prt10;
		uint16_t prt11;
	};
}RTCC;

struct TM_DATE_STRINGS
{                         //                             123456789 byte count for fromat 9 => null
   uint8_t hr_mins[9];    //format hours:mins:am or pm - hh:mm am
                          //                                1234567890
   uint8_t date[11];      //format day week, month, day ex) WED OCT 10
}; 
   
struct SET_TM;
struct SET_DATE;
struct TM_DATE_STRINGS;
uint8_t RTCC_get(RTCC *);
void RTCC_init(void);
void RTCC_set(struct TimePkt *,struct DatePkt *);
void build_tm_strings(struct TM_DATE_STRINGS *tm, RTCC *init, uint8_t includeYr);
uint8_t  mRTCCDec2Bin(uint8_t Dec);
uint8_t mRTCCBin2Dec(uint8_t Bin);

#endif
