/**
 * Filename: sensor.c
 *  Version: 1.0.0
 *
 *  support functions for sensor related data.
 *
 */
#include "sys_variables.h"
#include "dfformat.h"
#include "dataflash.h"
#include "s3vocequ.h"
#include "sensor.h"

/**
 *
 *  Copy sensor data from serial flash to ram.
 *  Note: method called when sensor text is received from panel 
 *
 */
void set_sensor_text(uint32_t flash_src, enum Stream_Id sensorN)
{
    extern struct NVDATA nvData;
    extern enum NVBACKUPSTATE nvBackupStatus;    
    uint16_t len;//i;
    uint8_t *dest;
    
    if(sensorN < all_sensor_text)
    {
        len = MAXNAMETOKENS;
        dest = &nvData.sensor_text[sensorN][0];
    }
    else
    {
        len = sizeof(nvData.sensor_text);
        dest = &nvData.sensor_text[0][0];                 
    }  
//    for(i=0;i < len;i++)
//    {
//        flash_read(flash_src+i,dest+i,1);
//    }
    flash_read(flash_src,dest,len);    
    set_recheck_present_flags();        //force a sensor type recheck
    check_name_token_range(dest, len);
    nvBackupStatus = NVBACKUP_START;
}

/**
 *
 * set the group numbers for a panel sensors 
 * 
 * Note: always loads all groups
 *
 */
void set_sensor_groups(uint32_t flash_src)
{
    extern struct NVDATA nvData;
    extern enum NVBACKUPSTATE nvBackupStatus;    
    
    flash_read(flash_src,nvData.sensor_group,sizeof(nvData.sensor_group));
    set_door_window_status();
    set_recheck_present_flags();        //force a sensor type recheck    
    nvBackupStatus = NVBACKUP_START;
}

/**
 *
 *  get the text tokens associated with znText[] (see sc10 uistring.c)
 *
 */
uint8_t get_zn_text(uint8_t buf[],uint8_t max,uint16_t phraseN)
{
    uint32_t address;
    uint8_t i;
    
    address = getZnTextAddress(phraseN); //get the address of the text string associated with phraseN
    for(i=0 ;flash_read(address+i,&buf[i],1), buf[i] && i < max ;i++);
    return i;
}

/**
 *
 *  get a sensor name from flash and convert it to text
 *  
 *
 */
uint8_t get_sensor_name(uint8_t dest[], uint8_t sensor, uint8_t max)
{        
   uint8_t j,i,token;

   for(i=j=0;sensor < NUMBER_OF_ZONES && j < MAXNAMETOKENS && i < (max-1); j++)
   {
      token = get_sensor_token(sensor,j);
      if(token == 0xff)
      {
         break;   
      }   
      i += get_zn_text(&dest[i],(max-1)-i,token);
   }
   dest[i] = 0;
   return i;
}

/**
 *
 *  convert sensor text token list to text
 *  
 *
 */
uint8_t get_sensor_text(uint8_t src[], uint8_t dest[], uint8_t max)
{        
   uint8_t j,i,token;

   for(i=j=0;j < MAXNAMETOKENS && i < (max-1); j++)
   {
      token = src[j];
      if(token == 0xff)
      {
         break;   
      }   
      i += get_zn_text(&dest[i],(max-1)-i,token);
   }
   dest[i] = 0;
   return i;
}

/**
 *
 * Get sensor speech token for given sensor number and index
 *
 */
uint8_t get_sensor_token(uint8_t sensorN, uint8_t index)
{
    extern struct NVDATA nvData;
        
    return nvData.sensor_text[sensorN][index];
}

/**
 *
 * get the status byte for the passed-in sensorN
 *
 */
union _CHANNEL_STATUS get_channel_status(uint8_t sensorN)
{
    extern struct VDATA vData;
    return vData.CHNDAT[sensorN];
}

/**
 *
 *
 * Check if passed in zone text index is associated
 * with a touchpad.
 * 
 *   See langENts.c const UISTRINGS znText[]
 *  
 *    return: 1 => znText[?] has the word door in it
 *            0 => znText[?] does not contain the word door
 * 
 *   
 */
uint8_t is_touchpad(uint8_t index)
{
    switch(index)     
    {
        case 0:                     //Keychain    
        case 1:                     //Touchpad
        case 159:                   //Keyfob
            return 1;
    }
    return 0;
}

/**
 *
 *
 * Check if passed in zone text index is associated
 * with a door.
 * 
 *   See langENts.c const UISTRINGS znText[]
 *  
 *    return: 1 => znText[?] has the word door in it
 *            0 => znText[?] does not contain the word door
 * 
 *   
 */
uint8_t is_door(uint8_t index)
{
    switch(index)     
    {
        case 136:                   //door - most likely index for the word door        
        case 2:                     //front door
        case 4:                     //back door
        case 6:                     //garage door
        case 23:                    //patio door
        case 132:                   //sliding door
        case 145:                   //side door
        case 160:                   //delay door
            return 1;
    }
    return 0;
}

/**
 *
 *
 * Check if passed in zone text index is associated
 * with a window.
 * 
 *   See UISTRINGS znText[] above.  
 *  
 *    return: 1 => znText[?] has the word door in it
 *            0 => znText[?] does not contain the word door
 * 
 */
uint8_t is_window(uint8_t index)
{
    switch(index)     
    {
        case 135:               //window - most likely index for the word window
        case 3:                 //front window
        case 5:                 //back window
        case 7:                 //garage window
        case 9:                 //master bedroom window
        case 11:                //bedroom window
        case 13:                //guest room window
        case 15:                //child's room window
        case 18:                //living room window
        case 20:                //kitchen window
        case 22:                //porch window
        case 25:                //office window
        case 27:                //den window
        case 31:                //basement window
        case 33:                //upstairs window
        case 35:                //downstairs window       
            return 1;
    }
    return 0;
 }
 
 /**
  *
  * check if sensor is a:
  *                 ADC module(token name = GSM Module) 
  *                         or a
  *                 IConbtrol module (token name = CONNMGR)
  *
  */
 uint8_t is_adc(uint8_t sensor)
 {
    if(sensor == 39)
    { 
        if(nvData.sensor_text[39][0] == 49 &&  //G see langents.c token name "G" 
            nvData.sensor_text[39][1] == 61 &&  //S see langents.c token name "S"
            nvData.sensor_text[39][2] == 55 &&  //M see langents.c token mame  "M"
            nvData.sensor_text[39][3] == 102 && //" " see langents.c token name " "
            nvData.sensor_text[39][4] == 41)    //module see langents.c token name "Module"
        {          
            return 1;
        }
        if(nvData.sensor_text[39][0] == 45 &&   //C see langents.c token name "C" 
            nvData.sensor_text[39][1] == 57 &&  //O see langents.c token name "O"
            nvData.sensor_text[39][2] == 56 &&  //N see langents.c token mame "N"
            nvData.sensor_text[39][3] == 56 &&  //N see langents.c token name "N"
            nvData.sensor_text[39][4] == 55 &&  //M see langents.c token name "M"
            nvData.sensor_text[39][5] == 49 &&  //G see langents.c token name "G"           
            nvData.sensor_text[39][6] == 60)    //R see langents.c token name "R"          
        {          
            return 1;
        }                      
    }            
    return 0;
 }
 
 /**
  *
  * check if sensor is a motion sensor
  *
  */
 uint8_t is_motion(uint8_t sensor)
 {
    extern struct NVDATA nvData;
    extern struct VDATA vData;
    
    if(is_adc(sensor) == 0) //check if sensor is an adc module
    {    
        switch(get_sensor_group(sensor))
        {  
            case 15:
            case 17:
            case 18:
            case 20:
            case 28:
            case 32:
                return 1;
        }                
    }
    return 0;
 }

/**
 *
 * Update the open/closed status of doors/windows and property in the system
 *
 */
void set_door_window_status(void)
{
    extern struct NVDATA nvData;
    extern struct VDATA vData;
    
    uint8_t i,x, isDoorOrWindowOrTp;
        
    vData.zn_state.doors_open = 0;
    vData.zn_state.windows_open = 0;
    vData.zn_state.other_open = 0;
    for(i=0; i < NUMBER_OF_ZONES;i++)
    {        
        if(vData.CHNDAT[i].chn_bits._open == 0) //not open so skip to next sensor
        {
            continue; 
        }
        if(is_motion(i))        //a motion sensor should never indicate open so skip to next sensor.
        {
            continue;   
        }            
        if(is_adc(i))           //adc module is not a door/window/motion or property sensor so skip to next sensor.
        {
            continue; 
        }
        for(x=0,isDoorOrWindowOrTp=0;x<MAXNAMETOKENS;x++)
        {
            if( nvData.sensor_text[i][x] == 0xff)       //check for end of text
            {
                if (x == 0)							//a sensor with no name is considered not programmed
				{
					isDoorOrWindowOrTp =1;			//setting isDoorOrWindowOrTp prevents the setting of vData.zn_state.other_open
				}						            //see line ~355-358
				break;                              
            }    
            if(is_window( nvData.sensor_text[i][x]))
            {
                isDoorOrWindowOrTp = 1;
                vData.zn_state.windows_open = 1;   
            }      
            if(is_door(nvData.sensor_text[i][x]))
            {
                isDoorOrWindowOrTp = 1;
                vData.zn_state.doors_open = 1;
            }
            if(is_touchpad(nvData.sensor_text[i][x]))
            {
                isDoorOrWindowOrTp = 1;               
            }             
        }
        if(isDoorOrWindowOrTp == 0)
        {
            vData.zn_state.other_open = 1;         
        }
        //if windows and door and other are open abort the for loop
        if(vData.zn_state.doors_open && vData.zn_state.windows_open && vData.zn_state.other_open)
        {
            break;                      //terminate outside for loop
        }                        
    }        
}

/**
 *
 * Check for door or window or touchpad or motion sensor
 * Input: Sensor ID (zone)
 *
 * Return: 1 if window or door or touchpad or motion, 
 *         0 if none
 *
 */
 uint8_t CheckIfWindowOrDoorOrTouchpadorMotion(uint8_t SensorID)
 {
     extern struct NVDATA nvData;
      
    uint8_t x;
    
    if(is_motion(SensorID))
    {
        return 1;
    }    
    for(x=0;x<MAXNAMETOKENS;x++)
    {
        if( nvData.sensor_text[SensorID][x] == 0xff)       //check for end of text
        {
            break;                              
        }    
        if(is_window( nvData.sensor_text[SensorID][x]))
        {
            return 1;
        }      
        if(is_door(nvData.sensor_text[SensorID][x]))
        {
            return 1;
        }
        if(is_touchpad(nvData.sensor_text[SensorID][x]))
        {
            return 1;
        }
    }       
    return 0;
 }
     
static struct 
{
    uint16_t motions:1;
    uint16_t recheck_motions:1;  
    uint16_t property:1;
    uint16_t recheck_property:1;
    uint16_t doors:1;
    uint16_t recheck_doors:1;
    uint16_t windows:1;
    uint16_t recheck_windows;
}sensor_present;

/**
 *
 * set re-check present flag
 *
 */
 void set_recheck_present_flags(void)
 {
    sensor_present.recheck_motions = 1;
    sensor_present.recheck_doors = 1;
    sensor_present.recheck_windows = 1;
    set_property_recheck(force_recheck);
    set_property_present(0);           
    set_motion_present(0);
   
    return;
 }
 
 /**
  *
  * set that a motion sensor rf packet has been received
  *
  */
void set_motion_present(uint8_t val)
{
    sensor_present.motions = val;    
}

/**
 *
 * set that a property sensor has been received
 *
 */ 
 void set_property_present(uint8_t val)
 {
    sensor_present.property = val;
 }    

/**
 *
 * return a sensors group
 *
 */
uint8_t get_sensor_group(uint8_t sensor)
{
    extern struct NVDATA nvData;
    if(sensor >= NUMBER_OF_ZONES)
    {
        return 0xff;
    }
    return nvData.sensor_group[sensor];              
}

/**
 *
 *
 * Check if any motion sensors present in system
 *  
 * 
 * Version 1.4 and later will check the group # 15,17,18,20,28 && 32
 *
 */
uint8_t motion_is_present(void)
{
    extern struct NVDATA nvData;
    uint8_t i;
    
    if(sensor_present.recheck_motions)
    {
        for(i=0;i < NUMBER_OF_ZONES;i++)
        {    
            set_motion_present(0);
            if(is_motion(i))
            {
                set_motion_present(1);
                i =  NUMBER_OF_ZONES;           //found a window sensor => force loop to end
            }
        }
    }
    sensor_present.recheck_motions=0;    
    return sensor_present.motions;
}

/**
 *
 * Check if any door sensors present in system
 *  
 * returns: 0 => door word token not found in sensor text
 *          1 => door word token found in sensor text
 */
uint8_t door_is_present(void)
{
    extern struct NVDATA nvData;
    uint8_t i,x;
    
    for(i=0;i < NUMBER_OF_ZONES && sensor_present.recheck_doors;i++)
    {    
        sensor_present.doors = 0;
        for(x=0;x<MAXNAMETOKENS;x++)
        {
            if(nvData.sensor_text[i][x] == 0xff)       //check for end of text
            {
                break;                              
            }        
            if(is_door(nvData.sensor_text[i][x]))
            {
                sensor_present.doors = 1;
                sensor_present.recheck_doors=0;         //terminate outside for(;;)loop 
                break;
            }   
        }
    }        
    sensor_present.recheck_doors = 0;    
    return sensor_present.doors;
}

/**
 *
 * Check if any window sensors present in system
 *  
 * returns: 0 => window word token not found in sensor text
 *          1 => window word token found in sensor text
 */
uint8_t window_is_present(void)
{
    extern struct NVDATA nvData;
    uint8_t i,x;
    
    for(i=0;i < NUMBER_OF_ZONES && sensor_present.recheck_windows;i++)
    {    
        sensor_present.windows = 0;
        for(x=0;x<MAXNAMETOKENS;x++)
        {
            if(nvData.sensor_text[i][x] == 0xff)       //check for end of text
            {
                break;                              
            }        
            if(is_window(nvData.sensor_text[i][x]))
            {
                sensor_present.windows = 1;
                sensor_present.recheck_windows=0;      //terminate outside for(;;)loop 
                break;
            }   
        }
    }        
    sensor_present.recheck_windows = 0;    
    return sensor_present.windows;
}

/**
 *
 * set property recheck flag
 *
 */
void set_property_recheck(enum SENSOR_PRESENT_RECHECK val)
{    
    if(val == force_recheck)
    {
       sensor_present.recheck_property = 1; 
    }
    else if(val == clear_recheck)
    {
       sensor_present.recheck_property = 0;     
    }                   
}

/**
 *
 * Check if any property sensors present in system
 *  
 * returns: 0 => no property sensors in system
 *          1 => property sensors in system
 *
 */
uint8_t property_is_present(void)
{
    extern struct NVDATA nvData;
    extern struct VDATA vData;
    uint8_t i;
      
    if(sensor_present.recheck_property)
    {
        set_property_present(0);    
        for(i=0;i < NUMBER_OF_ZONES && sensor_present.property==0;i++)
        {    
            if(nvData.sensor_group[i] == 43)     //check for asset group
            {    
                set_property_present(1);         
            }              
            if(nvData.sensor_text[i][0] == 0xff)                //sensor has no name so continue to next sensor
            {
                continue;
            }
            if(is_adc(i))                                       //check if sensor is an adc module
            {
                continue;
            }
            if(CheckIfWindowOrDoorOrTouchpadorMotion(i))    
            {
                continue;
            }
            //For SimonXT version 1.4 motions sensors are determined by group =>
            //if here the current sensor is not a motion sensor.
            set_property_present(1);                
        }
    }         
    set_property_recheck(clear_recheck);    
    return sensor_present.property;
}

#ifdef QC376_FIX
/**
 *
 * updata channel status and if channel status has moved from normal to alarm
 * set the alarm time stamp for the cooresponding zone.
 * 
 * input zone: 0-NUMBER_OF_ZONES+1
 * output: time stamp is set for given channel if channel has gone from normal state to alarm state.
 *
 */
void update_chn_status(uint8_t zone, uint8_t newChnData)
{
    union _CHANNEL_STATUS chnData;
    extern struct VDATA vData;
    RTCC currentTm;

    chnData.chnByte = newChnData;
    if (vData.CHNDAT[zone].chn_bits.inAlarm == 0 && chnData.chn_bits.inAlarm)
    {
        RTCC_get(&currentTm);
        vData.alarmTmStamp[zone].tm_min = currentTm.tm_min; 
        vData.alarmTmStamp[zone].tm_hour = currentTm.tm_hour;
    }
    vData.CHNDAT[zone].chnByte = chnData.chnByte;
    return;
}
#endif



#if 0
/**
 *
 * clear/set the alarm history bits in the CHNDAT
 * 
 *  inputs: clrAll: 1 => clear alarmbit and alarm history
 *                  0 => clear alarmbit and if alarmbit is set set alarm history bit
 */
void clrAlarmHistory(uint8_t clrAll)
{
    uint8_t i,mask;
    
    union _CHANNEL_STATUS chnStatus;  
    for(mask=0xfd,i=0;i < NUMBER_OF_ZONES+2;i++)
    {
        chnStatus = get_channel_status(i);
        if( clrAll )
        {
           chnStatus.chn_bits.inAlarm = 0;        
        }
        if( chnStatus.chn_bits.inAlarm )
        {
           chnStatus.chn_bits.inAlarm = 0;
           chnStatus.chn_bits.alarmHistory = 1;
        }
        else
        {
            chnStatus.chn_bits.alarmHistory = 0;    
        }
        set_channel_status(i, chnStatus.chnByte);                      
    }         
 }   
 #endif
