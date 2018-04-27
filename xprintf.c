/**
 * Filename: xprintf.c
 * Version: 1.0.0
 *
 * xprintf.c - sprintf like functionality used to 
 *             build strings buffer for display
 *
 */
#include "twttsdef.h"
#include "statusmanager.h"
#include "sensor.h"
#include "uimanager.h"
#include "dfformat.h"


/**
 * Itoa() converts nbrs upto 10000 into 5 digit ascii string
 */
uint8_t Itoa(uint8_t dest1[],uint16_t nbr,uint8_t precision)
{
   uint16_t tmp;
   uint8_t *dest;
   uint8_t i,len;
    
   dest = dest1;
   if(nbr & 0x8000)             //check for a negative number
   {
        nbr = ~nbr;             //two complement the negative number
        nbr++;
        dest[0] = '-';
        dest++;                 //point destination pointer past the negative sign
   }
   tmp = nbr;   
   dest[0] = (uint8_t)(nbr/10000+'0');
   nbr %= 10000;
   dest[1] = (uint8_t)(nbr/1000+'0');
   nbr %= 1000;
   dest[2] = (uint8_t)(nbr/100+'0');
   nbr %= 100;
   dest[3] = (uint8_t)(nbr/10+'0');
   nbr %= 10;
   dest[4] = (uint8_t)(nbr + '0');
   if(tmp < 10)
   {
      dest[0] = dest[4];
      len =1;
   }
   else if(tmp < 100)
   {
      dest[0] = dest[3];
      dest[1] = dest[4];
      len = 2;
   }
   else if(tmp < 1000)
   {
      dest[0] = dest[2];
      dest[1] = dest[3];
      dest[2] = dest[4];
      len =  3;
   }
   else if(tmp < 10000)
   {
      dest[0] = dest[1];
      dest[1] = dest[2];
      dest[2] = dest[3];
      dest[3] = dest[4];
      len =4;
   }
   else len = 5;
   if(precision && precision > len)
   {
      for(i=0;i < len;i++)
         dest[precision - (i+1)] = dest[len-(i+1)];
      for(i=0;i < (precision-len);dest[i++] = '0');
      len = precision;
   }
   return len + (dest != dest1);        //add one for negative sign position
}

uint8_t uint8toA(uint8_t dest[],uint8_t nbr)
{
    uint8_t tmp = nbr;
    
    dest[0] = (uint8_t)(nbr/100+'0');
    nbr %= 100;
    dest[1] = (uint8_t)(nbr/10+'0');
    nbr %= 10;
    dest[2] = (uint8_t)(nbr + '0');
    
    if(tmp < 10)
    {
        dest[0] = dest[2];
        dest[1] = 0;
        dest[2] = 0;
        return 1;
    }
    else if(tmp < 100)
    {
        dest[0] = dest[1];
        dest[1] = dest[2];
        dest[2] = 0;
        return 2;
    }
    else
        return 3;
}

uint8_t uint16toA(uint8_t dest[],uint16_t nbr)
{
    uint16_t tmp = nbr;
    
    dest[0] = (uint8_t)(nbr/10000+'0');
    nbr %= 10000;
    dest[1] = (uint8_t)(nbr/1000+'0');
    nbr %= 1000;
    dest[2] = (uint8_t)(nbr/100+'0');
    nbr %= 100;
    dest[3] = (uint8_t)(nbr/10+'0');
    nbr %= 10;
    dest[4] = (uint8_t)(nbr + '0');
    
    if(tmp < 10)
    {
        dest[0] = dest[4];
        dest[1] = 0;
        dest[2] = 0;
        dest[3] = 0;
        dest[4] = 0;
        return 1;
    }
    else if(tmp < 100)
    {
        dest[0] = dest[3];
        dest[1] = dest[4];
        dest[2] = 0;
        dest[3] = 0;
        dest[4] = 0;
        return 2;
    }
    else if(tmp < 1000)
    {
        dest[0] = dest[2];
        dest[1] = dest[3];
        dest[2] = dest[4];
        dest[3] = 0;
        dest[4] = 0;
        return 3;
    }
    else if(tmp < 10000)
    {
        dest[0] = dest[1];
        dest[1] = dest[2];
        dest[2] = dest[3];
        dest[3] = dest[4];
        dest[4] = 0;
        return 4 ;
    }
    else
        return 5;
}

/**
 *
 * fill memory with the valu specified by fill
 *
 */
 void memFill(uint8_t dest[], uint8_t fill, uint16_t count)
 {
    uint16_t i;
    for(i=0;i < count;dest[i]=fill,i++);    
    return;
 }       

/**
 * copy N char of a ram string src to dest
 */
uint8_t copyN(uint8_t *src, uint8_t *dest, uint8_t n)
{
   uint8_t i;
   for(i=0;i < n;dest[i] = src[i],i++);
   return i;
}

/**
 *
 * copy N char of a string rom src to dest
 *
 */
uint8_t copyNConst(const char *src, uint8_t *dest, uint8_t n)
{
   uint8_t i;
   for(i=0;i < n && src[i];dest[i] = src[i],i++);
   dest[i] = 0;
   return i;
}

uint8_t copyNConstToken(int token, uint8_t *dest, uint8_t n)
{
	uint8_t i;
   
	unsigned char out_tmp=0;
	unsigned char string_tmp[100];
	
	getToknToTxt(&out_tmp,sizeof(string_tmp),string_tmp,token);
   
	for(i=0;i < n && string_tmp[i];dest[i] = string_tmp[i],i++);
	dest[i] = 0;
	return i;
}

/**
 *
 *  get the text tokens associated with uiStrings[] (see sc10 uistring.c)
 *
 */
uint8_t get_uistrings_text(uint8_t dest[],uint8_t max,uint16_t phraseN)
{
    uint32_t address;
    uint8_t i;
    
    address = getUIStringsTextAddress(phraseN); //get the address of the text string associated with phraseN
    for(i=0 ;flash_read(address+i,&dest[i],1), dest[i] && i < max ;i++);
    return i;
}

/**
  inputs:
    format[]
        %A          = "A"cessCode field valu.
        %*A         = "A"cessCode field valu. '*' => replace accesscode digit with a '*'
        %B          = "B"it field valu.
        %C          = "C" com mode specifier.
        %oD         = "D"ecimal number format specifier.
		                o = 255 value is off
        %E          = Int"E"rnet Address specifiers.
        %F          = OnO"F"F format specifier.
        %I          = "I"ndex nbr(current virtual item idex ex. zn 1,2 or Ph 1 2).
        
        %niL       =  Event "L"og
                      n = index number (zero-padded 3-digit)
                      z = zone
                      i = information
        
        %H          = "H"ouse code specifier.
        %M          = Add Instant to arming level is on
        %N          = Accou"N"t account number '0-'9', 'A'-'F' left justified, 0xff padded
        %O          = H"O"me Control symbols (latchkey, chime 1, chime2,
        %P          = "P"hone number specifier.
        %R          = A"R"ming Level - show arming level
        %S          = ascii string '0'-'9' (used for downloader code).
        %ahmT       = "T"ime display the time.
                        a = blink am/pm
                        h = blink hours
                        m = blink minutes
        %V          = display revision, including patch number format n.n.n,
                      the last n is not displayed if there is no patch.
        %X          = hex string
       %cgnptxZ   = "Z"one number format specifier.
                        n = name modifier
                        c = condition of sensor modifier: alarm, lbat, trouble,
                                        bypassed,fire,open,closed.
                        g = optional group modifier
                        r = show rounds detected (used in sensor test mode)
                        t = optional sensor "t"ype modifier
                        x = show x10 mapping

    max = maximum number of bytes that may placed in
          dest[]
 */
uint16_t xprintf(uint8_t dest[],uint8_t max, struct PHRASE_DATA *pData, uint8_t fontSize, uint8_t fontStyle, uint16_t maxPixelWidth)
{
    extern struct VDATA vData;
    union _CHANNEL_STATUS chn_status;
    //struct CHN_BITS chn_status;
    uint8_t i,out,format_char;
    uint32_t address,address_i1;
    uint8_t tmpString[50];
    uint16_t tmpStringLen;
//    uint16_t stringWidth;
    struct STRING_PARAMS stringParams;
    struct STRING_PARAMS stringParams2;
    uint8_t shiftStatusCharacters = 0;
    uint8_t sensorEndIndex = 0xff;
    
    stringParams.size = fontSize;
    stringParams.style = fontStyle;
    
    stringParams2.size = fontSize;
    stringParams2.style = fontStyle;
    
    stringParams.pString = (char*)dest;
    stringParams2.pString = (char*)tmpString;

    if(pData->phraseN == T_None1)
    {
        return 0;
    }      
    address = getUIStringsTextAddress(pData->phraseN);
    for(out=i=0;flash_read(address,&format_char,1),format_char && out < max;address++)
    {       
        switch(format_char)
        {
            case '%':              //check for format specifier
                //scan past any modifiers
                for(address++,address_i1=address;flash_read(address,&format_char,1), format_char < 'A' || format_char > 'Z';address++);
                switch(format_char)
                {
                   /**********************************
                    check for accesscode specifier
                    No support for accesscode in twtts
                    **********************************/
#if 0
                    case 'A':
                   /**********************************
                    check for bit field data
                    **********************************/
                    case 'B':
                   /********************************
                     Ethernet address
                     No comm mode support in twtts
                    ********************************/
                    case 'C':
                   /*********************************
                    check for decimal digit specifier
                    *********************************/
                    case 'D':
                   /********************************
                     Ethernet address
                     No Ethernet support in twtts
                    ********************************/
                    case 'E':
                    case 'F':
                   /*********************************
                    X10 house code 
                    No House code support in twtts
                    *********************************/    
                    case 'H':
                        break;
#endif                        
                   /*******************************
                     I show the index
                    *******************************/      
                    case 'I':
                        out += Itoa(&dest[out],pData->generic+1,0);
                        break;
#if 0                        
                   /*********************************
                     check for Event Log
                    *********************************/
                    case 'L':
                        for ( lstOut = out; i1 < i; i1++, lstOut = out )
                        {
                           unsigned char type;

                           switch ( format[i1] )
                           {
                              case 'n': /* index number: "#03" */
                                 dest[out++] = '#';
                                 out += Itoa(&dest[out],dataEntry.data.event.index, 3);
                                 break;
                              case 'i': /* info: "Front Door Alarm" */
                                 xprintf_zoneinfo(&out, max, buf);
                                 break;
                           }
                        }
                        break;                    
                   /**********************************
                    Check for instant
                    **********************************/
                    case 'M':
                   /**********************************
                    Check for account code specifier
                    No account code support in twtts
                    *******************************/
                    case 'N':
                   /*******************************
                    Check if displaying
                    home control symbols
                    *******************************/
                    case 'O':
                   /*******************************
                    check for phone Number specifier
                    No phone number support in twtts
                    *******************************/
                    case 'P':
                        break;
                   /*********************************
                    check if arming level should be
                    displayed
                    *********************************/
#endif                    
                    case 'R':
                        switch( vData.panel_status.arming_lvl )
                        {
                            case subdisarmed:  //0 can't happen... but account for it
                                out += get_uistrings_text(&dest[out],max-out,T_SubDisarmed);
                            case disarmed:
                                //if(NVB_BITOPTIONS.FULLDISARMSTAT)
                                out += get_uistrings_text(&dest[out],max-out,T_Disarmed);
                                break;
                            case doors:
                                out += get_uistrings_text(&dest[out],max-out,T_DrWindowOn);
                                break;
                            case motions:
                                out += get_uistrings_text(&dest[out],max-out,T_MotionsOn);
                                break;
                            case doors_motions:
                                out += get_uistrings_text(&dest[out],max-out,T_DoorWndMot);
                                break;
                        }
                        break;
#if 0                        
                   /*********************************
                    check for string format specifier
                    Specifier: %S
                    modifiers:
                    *********************************/
                    case 'S':
                        for(i1=0;i1 < cur->strnLen && cur->data.generic[i1];i1++)
                            dest[out++] = cur->data.generic[i1];
                        if(cur->editMode)
                            out += blink(&dest[out],0);
                        break;
                   /***************************************************
                    check for time specifier
                    Specifier: %T
                    modifiers: s =>start time
                           e =>end time
                           t => time only (not a time interval)
                    Note: unprogrammed min and hour valu = 0xff
                    ***************************************************/
                    case 'T':
                        if(format[i1] == 'e')
                        {
                            edit = (cur->editMode && cur->dataType != TIME_FLD)?1:0;
                            hr = dataEntry.data.sched.time[1].hr;
                            min = dataEntry.data.sched.time[1].min;
                        }
                        else
                        {
                            //If time of day not set and not in scroll mode flash time.
                            if(sbBitFlags.F_scrollMenu == 0 && sbBitFlags.F_TIMESET == 0 && NVB_BITOPTIONS.DEMOMODE == 0)
                            {
                                edit = 0;
                                out += blink(&dest[out],1);
                            }
                            else
                                edit = (cur->editMode && cur->dataType==TIME_FLD)?1:0;
                            hr =/*dataEntry.*/cur->data.sched.time[0].hr;
                            min = /*dataEntry.*/cur->data.sched.time[0].min;
                        }
                        if(format[i1] == 'e' && hr == 0xff)
                            dest[out++] = ' ';  //provide a space to separate --:-- from the dash between start and end time
                        if(edit && cur->max == 23)
                            out += blink(&dest[out],1);
                        tmp = hr;
                        if(rdNvBit(NVBIT__24HOURCLOCK) == 0)
                        {
                            if(!tmp)
                                tmp = 12;
                            else if(tmp > 12)
                                tmp -= 12;
                        }
                        if(hr == 0xff)
                        {
                            dest[out++] = '-';
                            dest[out++] = '-';
                        }
                        else
                           out +=Itoa(&dest[out],tmp,2);
                        if(edit && cur->max == 23)
                            out += blink(&dest[out],0);
                        dest[out++] = ':';
                        if(edit && cur->max == 59)
                            out += blink(&dest[out],1);
                        if(min == 0xff)
                        {
                            dest[out++] = '-';
                            dest[out++] = '-';
                        }
                        else
                            out += Itoa(&dest[out],min,2);
                        if(edit && cur->max == 59)
                            out += blink(&dest[out],0);
                        if(format[i1] == 's' && min == 0xff)
                            dest[out++] = ' ';  //provide a space to separate --:-- from the dash between start and end time
                        if(rdNvBit(NVBIT__24HOURCLOCK) == 0 && min != 0xff && hr != 0xff)
                        {
                           tmp = T_AM;
                           if(hr > 11)
                               tmp = T_PM;
                           if(edit && cur->max == 1)
                               out += blink(&dest[out],1);
                           getToknToTxt(&out,max,buf,tmp);
                           if(edit && cur->max == 1)
                                out += blink(&dest[out],0);
                        }
                        //If time of day not set and not in scroll mode flash time.
                        if(sbBitFlags.F_scrollMenu == 0 && sbBitFlags.F_TIMESET == 0)
                        {
                            out += blink(&dest[out],0);
                        }
                        break;
                    /*******************************
                     Output System revision
                    *******************************/
                    case 'V':
                       dest[out++] = (uint8_t)(REV>>4) + '0';
                       dest[out++] = '.';
                       dest[out++] = (uint8_t) (REV & 0xf) + '0';
                       if (patchNumber != 0xff)
                       {
                           dest[out++] = '.';
                           //bcd  = patchNumber & 0xf;
                           dest[out++] = (patchNumber < 10) ? patchNumber+'0':patchNumber-10+'A';
                       }
                       break;
                    /*******************************
                     check for hex string
                     length specified by
                     dataEntry.strLen
                     *******************************/
                    case 'X':
                       for(i1=0;i1 < cur->strnLen;i1++)
                       {
                           out += Htoa(&dest[out],cur->data.generic[i1]);
                       }
                       break;
                    /***********************************
                     * Check for date specifier.
                     * Specifier: %Y   Modifiers: t
                     *  %tY  display date
                     ************************************************/
                     case 'Y':
                        for ( lstOut = out; i1 < i; i1++, lstOut = out )
                        {
                           switch ( format[i1] )
                           {          
                              case 't': /* current date format for events */
                                 out += dt_strfdate(&dest[out], max - out,&dataEntry.data.event.record.datetime);
                                 break;   
                           }
                        }
                        break;                            
#endif                       
                   /*******************************
                    check for sensor zone specifier
                    *******************************/
                    case 'Z':
                        chn_status.chnByte = pData->generic2; //get_channel_status(pData->generic).chn_bits;                    
                        for(;address_i1 < address;address_i1++)
                        {
                            switch(flash_read(address_i1,&format_char,1),format_char)
                            {
                                case 'c':         //display condition of sensor
                                    /*if(chn_status.inAlarm)
                                    {
                                        out += get_uistrings_text(&dest[out],max-out,T_SensorAlarm);
                                    }
                                    if(chn_status.lowBat)
                                    {
                                        out += get_uistrings_text(&dest[out],max-out,T_LowBat);
                                    }
                                    if(chn_status.alarmHistory && chn_status.inAlarm == 0)
                                    {
                                        out += get_uistrings_text(&dest[out],max-out,T_SensorAlarm);
                                    }
                                    if(chn_status.bypassed)
                                    {
                                        out += get_uistrings_text(&dest[out],max-out,T_SensorBypass);;
                                    }
                                    if(chn_status.supervisory)
                                    {
                                        out += get_uistrings_text(&dest[out],max-out,T_SensorFail);
                                    }
                                    if(chn_status._open) //twtts does not have group info && cur->data.zone.group.b_Grp.sirenControl !=2
                                    {
                                        out += get_uistrings_text(&dest[out],max-out,T_OpenSensor);
                                    }
#if 0                                    
                                    if(chn_status._open)//twtts does not have group info && cur->data.zone.group.b_Grp.sirenControl ==2
                                    {
                                        out += get_uistrings_text(&dest[out],max-out,T_SmokeWarningSensor);
                                    }
#endif                                    
                                    if(chn_status.tampered)
                                    {
                                        out += get_uistrings_text(&dest[out],max-out,T_TamperSensor);
                                    }
                                    break;*/
                                    if(chn_status.chn_bits.inAlarm)
                                    {
                                        tmpStringLen = get_uistrings_text(tmpString,50,T_SensorAlarm);
                                    }
                                    if(chn_status.chn_bits.lowBat)
                                    {
                                        tmpStringLen = get_uistrings_text(tmpString,50,T_LowBat);
                                    }
                                    if(chn_status.chn_bits.alarmHistory && chn_status.chn_bits.inAlarm == 0)
                                    {
                                        tmpStringLen = get_uistrings_text(tmpString,50,T_SensorAlarm);
                                    }
                                    if(chn_status.chn_bits.bypassed)
                                    {
                                        tmpStringLen = get_uistrings_text(tmpString,50,T_SensorBypass);;
                                    }
                                    if(chn_status.chn_bits.supervisory)
                                    {
                                        tmpStringLen = get_uistrings_text(tmpString,50,T_SensorFail);
                                    }
                                    if(chn_status.chn_bits._open)// twtts does not have group info && cur->data.zone.group.b_Grp.sirenControl !=2
                                    {
                                        tmpStringLen = get_uistrings_text(tmpString,50,T_OpenSensor);
                                    }
#if 0                                    
                                    if(chn_status.chn_bits._open)//twtts does not have group info && cur->data.zone.group.b_Grp.sirenControl ==2
                                    {
                                        tmpStringLen = get_uistrings_text(tmpString,50,T_SmokeWarningSensor);
                                    }
#endif                                    
                                    if(chn_status.chn_bits.tampered)
                                    {
                                        tmpStringLen = get_uistrings_text(tmpString,50,T_TamperSensor);
                                    }
                                    
                                    for(; out > 0; out--)
                                    {
                                        dest[out] = 0;
                                        
                                        if (((getStringWidth(&stringParams) + getStringWidth(&stringParams2)) <= maxPixelWidth) && 
                                            ((out + tmpStringLen) <= max))
                                            break;
                                    }
                                    strncpy((char*)&dest[out], (char*)tmpString, tmpStringLen);
                                    out += tmpStringLen;
                                    break;
                                case 'n':   //convert txt tokens to Txt                                
                                    out += get_sensor_name(&dest[out],pData->generic, max-out-1);   
                                    
                                    for(; out > 0; out--)
                                    {
                                        dest[out] = 0;
                                        
                                        if ((getStringWidth(&stringParams) <= maxPixelWidth) && (out <= max))
                                            break;
                                    }
                                    sensorEndIndex = out;
                                    break;
                                case 'g':       //get the group number... No support for twtts
                                    break;
                                case 'r':       //display rnds detected - not supported
                                    break;
                                case 'x':       //show x10... No support in twtts
                                    break;
                            }
                        }
                    break;
                }
                break;
            default:
                dest[out++] = format_char;
                if ((sensorEndIndex != 0xff) && ((out >= max) || (getStringWidth(&stringParams) > maxPixelWidth)))
                {
                    shiftStatusCharacters++;
                    for (i=sensorEndIndex-shiftStatusCharacters; i<max-1; i++)
                        dest[i] = dest[i+1];
                    out--;
                }
                break;
        }         
    }
    dest[out] = 0;
    return out;
}
