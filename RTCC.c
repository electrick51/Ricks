
#include "twttsdef.h"
#include "sys_variables.h"
#include "rtcc.h"

/*********************************************************************
 * Function: RTCC_init
 *
 * Preconditions: RTCCInit must be called before.
 *
 * Overview: Enable the oscillator for the RTCC
 *
 * Input: None.
 *
 * Note: This function has legacy which at one point set a hardware
 *       based real-time clock
 ********************************************************************/
void RTCC_init(void)
{
   struct TimePkt time;
   struct DatePkt date;

   // Write the default time and date to RTCC as follows. 
	time.tm_sec = 0x00;
	time.tm_min = 0;
	time.tm_hour = 12;
	date.tm_wday = 5; 
	date.tm_mday = 1;
	date.tm_mon = 1;
    date.tm_year = 11;
   // Set RTCC after you change the time and date. 
	RTCC_set(&time,&date);
}

/**
 *
 * Convert to packed bcd to decimal
 *
 */
uint8_t  mRTCCDec2Bin(uint8_t Dec)
{
    return (10*(Dec>>4))+(Dec&0x0f);  //convert packet bcd to decimal
}

/**
 *
 * Convert decimal to bcd
 *
 */
uint8_t mRTCCBin2Dec(uint8_t Bin)
{
    return ((Bin/10)<<4) | (Bin%10);  //convert decimal to packet bcd
}

/*********************************************************************
 * Function: RTCC_set 
 *
 * Preconditions: None.
 *
 * Overview: 
 * The function upload time and date from _time_chk into clock.
 *
 * Input: _time_chk - structure containing time and date.
 *
 * Output: None.
 *
 * Note: This function has legacy which at one point set a hardware
 *       based real-time clock
 ********************************************************************/
void RTCC_set(struct TimePkt *time, struct DatePkt *date)
{
	extern struct VDATA vData;
    
    vData.rtc.tm_sec = mRTCCBin2Dec(time->tm_sec);
	vData.rtc.tm_min = mRTCCBin2Dec(time->tm_min);          //rtcc minute 0-59
	vData.rtc.tm_hour = mRTCCBin2Dec(time->tm_hour);        //rtcc hours 0-23
    vData.rtc.tm_wday = mRTCCBin2Dec(date->tm_wday);        //RTCC stores wday 0-6 (0=sunday)
    vData.rtc.tm_mday = mRTCCBin2Dec(date->tm_mday);        //rtcc stores mday 1-xx
    vData.rtc.tm_mon = mRTCCBin2Dec(date->tm_mon+1);        //rtcc stores month 1-12
    vData.rtc.tm_year = mRTCCBin2Dec(date->tm_year % 100);  //rtcc years 0-99    
}

/*********************************************************************
 * Function: RTCC_get
 *
 * Preconditions: RTCCInit must be called before.
 *
 * Overview: The function grabs the current time from the RTCC and
 * translate it into strings.
 *
 * Input: None.
 *
 * Output: It update time and date strings  _time_str, _date_str,
 * and _time, _time_chk structures.
 *
 *  return: current minute
 * 
 * Note: This function has legacy which at one point read a hardware
 *       based real-time clock
 ********************************************************************/
uint8_t RTCC_get(RTCC *dest)
{
	extern struct VDATA vData;
    *dest = vData.rtc;
    return vData.rtc.tm_min; 
}

const char *AM_PM[] = {"a ", "p "};

void build_tm_strings(struct TM_DATE_STRINGS *tm, RTCC *init, uint8_t includeYr)
{
    extern struct VDATA vData;
    RTCC tm_date; 
    uint8_t hours;
 //  struct TM_DATE_STRINGS tm;
                                                                                                           
    if (init)
    {
        tm_date = *init;
    }
    else
    { 
        tm_date = vData.rtc;        //get the time and date from the real time clock/calender
    }
    hours = tm_date.tm_hour;
    //
    //check if using 12 hour format
    //if using 12 hour format adjust hours as follows
    //
    //
    //    24 hour bcd time to 12hour format translation table
    //am                                                                 
    //x0=12am,x1=1,x2=2,x3=3,x4=4,x5=5,x6=6,x7=7,x8=8,x9=9,x10=10,x11=11,
    //pm
    //x12=12pm,x13=1,x14=2,x15=3,x16=4,x17=5,x18=6,x19=7,x20=8,x21=9,x22=10,x23=11 
    //
    if( vData.misc_panel._24hr_format == 0)
    {
   
        //translate 0x22-0x23 to 0x10 to 0x11 (10pm to 11pm)
        if(hours >= 0x22)
        {
            hours -= 0x12;
        }
        //translate 0x20-0x21 to 0x8 thru 0x9 (8pm to 9 pm)
        else if(hours > 0x19)
        {
            hours -= 0x18;
        }
        //translate 0x13-0x19 to 0x1 thru 0x7 (1pm to 7 pm)
        else if(hours > 0x12)
        {
            hours -=0x12;  
        }   
        //translate 0x0 hours to 12am
        else if(hours == 0)                //hours = 0 => 12am   
        {
            hours = 0x12;                      
        }
        //
        // AM or PM     0123456NULL=7  ex) 12:59P
        // Time Format - hh:mmp
        memcpy(&tm->hr_mins[5],AM_PM[tm_date.tm_hour > 0x11],strlen(AM_PM[tm_date.tm_hour > 0x11])+1); //+1 to include null character        
    }
    else
    {
        tm->hr_mins[5] = 0;     //for 24 hour format null terminate the string after minutes location (no am or pm indication)
    }     
    //hours
    tm->hr_mins[0] = (hours >> 4) + '0';
    tm->hr_mins[1] = (hours & 0xf) + '0';
    tm->hr_mins[2] = ':';
    //minutes
    tm->hr_mins[3] = (tm_date.tm_min >> 4) + '0';
    tm->hr_mins[4] = (tm_date.tm_min & 0xf) + '0';   
    //
    //                      01234NULL=5     012345
    //Domestic date format: mm/dd        ex)07/13NULL
    //Europe  date format:  dd/mm        ex)13/07NULL

    // check if date order is month/day or day/month
    if( vData.misc_panel.day_month_order == 0)
    {    
       //month
        tm->date[0] = (tm_date.tm_mon >> 4) + '0';
        tm->date[1] = (tm_date.tm_mon & 0xf) + '0'; 
        //separator
        tm->date[2] = '/';
        //day
        tm->date[3] = (tm_date.tm_mday >> 4) + '0';
        tm->date[4] = (tm_date.tm_mday & 0xf) + '0'; 
    }         
    else
    {
        //day
        tm->date[0] = (tm_date.tm_mday >> 4) + '0';
        tm->date[1] = (tm_date.tm_mday & 0xf) + '0';             
        //separator
        tm->date[2] = '/';
        //month
        tm->date[3] = (tm_date.tm_mon >> 4) + '0';
        tm->date[4] = (tm_date.tm_mon & 0xf) + '0';

    }
    tm->date[5] = 0;                          //null terminate
    if (includeYr)
    {    
        //year
        tm->date[5] = '/';
        tm->date[6] = (tm_date.tm_year >> 4) + '0';
        tm->date[7] = (tm_date.tm_year & 0xf) + '0';   
        tm->date[8] = 0;
    }
    return; 
}   
