/**
 * Filename: sys_variables.c
 *  Version: 1.0.0
 *
 *  declare system wide and flash-backed variables
 *
 */
#include "twttsdef.h"
#include "queue.h"
//#include "graphics\touchscreen.h"
//#include "mainscreen.h"
#include "xprintf.h"
#include "sys_variables.h"
#include "dfformat.h"
#if 0 //KGS removed audio.c
#include "audio.h"
#endif
//#include "dealercontactscreen.h"
#include "statusmanager.h"
//#include "eventscreen.h"
//#include "graphics\graphics_app_layer.h"
//#include "statusscreen.h"
//#include "faq_screen.h"
//#include "ad_screen.h"
//#include "eventscreen.h"
#include "resources.h"
//#include "jpeg.h"
#include "rftrx.h"

struct SYS_STATUS sys_status;
struct PROTEST_INFO protest_info;               //contains protest data sent by the panel
uint8_t scratch_buffer[SCRATCH_BUFFER_SZ];      //general scratch buffer...contents should be assumed changed after a context switch
struct MOTIONLIST MotionList[NUMBER_OF_ZONES];   // List of motions to display

enum NVBACKUPSTATE nvBackupStatus;
struct NVDATA __attribute__((__section__(".nvdata"))) nvData;
struct MFGDATA __attribute__ ((__section__(".mfgdata"))) mfgData;
struct VDATA vData;         //volatile system variables
struct SENSORQUE sensorque[SENSOR_BUFFER_SIZE];


//extern uint8_t tpid1;
//extern uint8_t tpid2;
//extern uint8_t tpid3;
//extern uint8_t teky5;

//void set_default_light_and_lock_names(void)
//{
//    uint8_t i,ii;
//    uint8_t light_string[] = {54,111,109,110,122,102,69};
//    
//    for (i=0;i<sizeof(light_string)-1;i++)
//    {
//        for (ii=0;ii<NUMBER_OF_LIGHTS;ii++)
//        {
//            nvData.light_data.light_name[ii][i] = light_string[i];
//        }
//    }
//    for (ii=0;ii<NUMBER_OF_LIGHTS;ii++)
//    {
//        nvData.light_data.light_name[ii][sizeof(light_string)-1] = light_string[sizeof(light_string)-1]+ii;
//    }
//    for (ii=0;ii<NUMBER_OF_LIGHTS;ii++)
//    {
//        nvData.light_data.light_name[ii][sizeof(light_string)] = 0xFF;
//    }
//    
//    for (ii=0;ii<NUMBER_OF_DOORLOCKS;ii++)
//    {
//        nvData.doorlock_data.doorlock_sensor[ii] = 0xff;
//    }
//}

/**
 *
 *  on startup restore NVD with content from the serial flash
 *
 */

/**
 *
 * default the download data
 *  1) ads[]
 *  2) faq[]
 *  3) dealer_contact_msg[]
 *  4) service_provider_msg[]
 *  
 */
//void default_msg_dwnloads(void)
//{
//    extern struct NVDATA nvData;
//    
//    uint8_t i;
//    //
//    // clear all ads
//    //
////    for(i=0;i <= MAX_ADS ;i++)
//    {
////        set_ad_msg(i,0,0);
//    } 
//    //
//    // set faq string to 0xff
//    //
//    memFill((uint8_t *) nvData.faq, 0xFF, sizeof( nvData.faq));  
//    //
//    // set dealer contact message to 0xff
//    //
//    memFill((uint8_t *) nvData.dealer_contact_msg, 0xFF, sizeof( nvData.dealer_contact_msg));
//    //
//    // set provider message to 0xff
//    //
//    memFill((uint8_t *) nvData.service_provider_msg, 0xFF, sizeof( nvData.service_provider_msg));    
//    nvBackupStatus = NVBACKUP_START;      
//}

/**
 *
 * when a twtts is enrolled default the following
 *  1) logo_index = 1
 *  2a) sensor text
 *  2b) sensor groups
 *  3) thermostat info
 *  4) weather info
 *  5) event buffer content
 *  6) brightness == 200
 *  7) volume == 8
 *  8) dealer_contact
 *  9) faq[]
 * 10) ad[]
 */
//void enroll_defaults(void)
//{
////    set_audio_volume(8);
////    set_beep_volume(AudioLevel4);    
////    set_brightness(200);
////    set_default_screen(key_scn_main);
//////    set_chime(1);
//////    set_secure_ts(0);
////    //
////    // clear downloaded images
////    //
////    default_msg_dwnloads();
//    //
//    //clear sensor text
//    //
//    memFill( &nvData.sensor_text[0][0],0xff,sizeof(nvData.sensor_text));
//    //
//    //clear sensor groups
//    //
//    memFill(nvData.sensor_group,0xff,sizeof(nvData.sensor_group));    
//    //
//    //clear light name
//    //
////    memFill( &nvData.light_data.light_name[0][0],0xff,sizeof(nvData.light_data.light_name));
//    memFill( &nvData.light_data.enable_mask,0x00,sizeof(nvData.light_data.enable_mask));
//    //
//    //clear doorlock name
//    //
////    memFill( nvData.doorlock_data.doorlock_sensor,0xff,sizeof(nvData.doorlock_data.doorlock_sensor));
//    memFill( &nvData.doorlock_data.enable_mask,0x00,sizeof(nvData.doorlock_data.enable_mask));
////    set_default_light_and_lock_names();
//    //
//    //set the log index to the no logo
//    //
////    nvData.logo_index = NO_LOGO;   // 0=PINNACLE_LOGO, 1=ge logo, 2=DFI_DEFENDER_LOG0, 3= ge logo, 0xff= no logo
//    nvBackupStatus = NVBACKUP_START;
//    //
//    // flush the event buffer
//    //
////    event_buffer_flush();
//}

/**
 *
 *	Initialize RAM data in .nvdata section to defaults
 *
 */
//void nvDefault(void)
//{
//    // set default values for all nvData member variables here
//    // no need to call set functions here
//     
//    enroll_defaults();
//    //
//    // set calibration offsets to default values
//    // NOTE: This is also done after every reset right now because there is no way
//    //       to cause a cold reset in case the calibration gets messed up
//    //  RGS 1/29/16 Modified for 480 x 272 display
//    
//    nvData.cal.lh_offset = -35;//-37;
//    nvData.cal.uh_offset = -33;//-35;
//    nvData.cal.lv_offset = -46;//-44;
//    nvData.cal.uv_offset = -47;  //QC549
//    nvData.cal.lh_factor = 7249;//7359;     //7255;//5000;
//    nvData.cal.uh_factor = 7216;//7196;     //5000;
//    nvData.cal.lv_factor = 5103;//5097;     //4300;
//    nvData.cal.uv_factor = 5087;//5218;     //4300;
//
//    //nvData.trigValLo=1; // moved out of NV space - gmk
//    //nvData.trigValHi=0;
//    nvData.panel_firmware_ver=0;
//    // NOTE:  Recalculation of CRC should always be the last line in this function
//    nvData.CRC = calculatedCRC();
//    // No code below here!
//}

/**
 *
 *  Check whether nvData section of RAM needs to be backed up and if so, do it
 *
 */
//void nvBackupChk(void)
//{
//    static uint16_t nvBackupInitTime;
//
//	// check nvBackupStatus
//	// set nvBackupStatus = NVBACKUP_START if you want RAM to be backed up after changing an NV variable
//    switch (nvBackupStatus)
//    {
//        case NVBACKUP_IDLE:
//            break;
//        case NVBACKUP_START:
//            // start a 10-second timer
//            nvBackupInitTime = timers_seconds_get();
//            nvBackupStatus = NVBACKUP_TIMING;
//            break;
//        case NVBACKUP_TIMING:
//            // do backup after recalculating CR
//            if (timers_seconds_elapsed(nvBackupInitTime) > 10)
//            {
//                nvData.CRC = calculatedCRC();
////                nvCopyToFlash();
//                nvBackupStatus = NVBACKUP_IDLE;
//            }
//            break;
//        default:
//            nvBackupStatus = NVBACKUP_IDLE;
//            break;
//    }
//}

/**
 *
 *  Copy nvData to flash
 *  assumes CRC is ok!!
 *
 */
//void nvCopyToFlash(void)
//{
//    sFlashEraseSectors(getNVDStart(), sizeof(nvData));    
//    flash_write(getNVDStart(), (unsigned char *) &nvData, sizeof(nvData));
//}

/**
 *
 *  Restore RAM from flash
 *
 */
void nvRestoreFromFlash(void)
{
//    flash_read(getNVDStart(), (uint8_t *) &nvData, sizeof(nvData));
}

/**
 *
 *  Copy mfgData to flash
 *
 */
void mfgCopyToFlash(void)
{
//    sFlashEraseSectors(getMfgStart(), sizeof(mfgData));     
//    flash_write(getMfgStart(), (unsigned char *) &mfgData, sizeof(mfgData));
}


/**
 *
 *  Restore manufacturing data from flash
 *
 */
void mfgRestoreFromFlash(void)
{
//    flash_read(getMfgStart(), (uint8_t *) &mfgData, sizeof(mfgData));
    // verify legal tpid
    // 80 bit sensors 0x00007<id<0xfffff else illegal
//    if (((0xff == mfgData.id1) && (0xff == mfgData.id2) && (0xf0 == (mfgData.id3 & 0xf0)))
//        || ((0x00 == mfgData.id1) && (0x00 == mfgData.id2) && ((mfgData.id3 & 0xf0) <= 0x70)))
//        {
//        return;
//        }
//    else
//       {
//        nvData.tpid1 = mfgData.id1;
//        nvData.tpid2 = mfgData.id2;
//        nvData.tpid3 = mfgData.id3;
//        nvData.teky5 = mfgData.key5;
//        }
//
//
//
//    tpid1 = mfgData.id1;
//    tpid2 = mfgData.id2;
//    tpid3 = mfgData.id3;
//    teky5 = mfgData.key5;
}

/**
 *
 *  on startup tweak NVD variables with a _x suffix that aren't really non-volatile
 *
 */
void xInit(void)
{
    // put special assignments here
    // do not use set functions that cause recalculation of CRC.

    // kludge calibration stuff
#if 0
    nvData.cal.lh_offset = -40;
    nvData.cal.uh_offset = -40;
    nvData.cal.lv_offset = -40;
    nvData.cal.uv_offset = -40;
    nvData.cal.lh_factor = 5000;
    nvData.cal.uh_factor = 5000;
    nvData.cal.lv_factor = 4300;
    nvData.cal.uv_factor = 4300;
#endif        
}

/**
 *
 *  on startup initialize some volatile variables
 *
 */
void vInit(void)
{    
   // void set_rf_dlink_ok( void );

   // set_rf_dlink_ok();

    vData.panel_status.arming_lvl = 1;
    //
    // set thermostat information to all 0xff
    //
//    memFill((uint8_t *) &vData.thermostat[0],0xff,sizeof(vData.thermostat));
    //
    // set weather info to 0xFFs
    //
//    memFill((uint8_t *) &vData.weather[0], 0x80, sizeof(vData.weather));
    //
    // set last weather update time strings to blank
    //
//    memFill((uint8_t *) &vData.last_weather_update_tm, 0x00, sizeof(vData.last_weather_update_tm));     
         
#if 0//KGS for test only

//    nvData.secure_ts = 1;
    vData.misc_panel.interactive_services = 1;  //KGS remove
    nvData.panel_firmware_ver = SIMONXTVER14;
#if 1   //there are defines here thaat will enable numerous testing features/screens/data
        //I have also tried to add a way to remove any test data so that the testing data is not permanent once created
     uint8_t i;   
//***************************************************************************************************************************
//USE THIS AREA TO CHOOSE WHICH TESTING DATA YOU WOULD LIKE TO ENABLE/DISABLE/CLEAR

    #define createFAQ                   //uncomment to enable FAQ page
      //  #define clearFAQ                      //uncomment to remove FAQs
    #define createDEALERcontact         //uncomment to enable dealer contact page
        //#define clearDEALERcontact            //uncomment to remove dealer contact
    #define createADpage                //uncomment to enable ads
       // #define clearADpage                   //uncomment to disable ads
    #define createDOORlocksANDlights    //uncomment to enable lights and locks
    //#define clearDOORlocksANDlights       //uncomment to disable lights and locks
    //#define setCALIBRATIONvalues        //uncomment to set calibration values
    #define createEVENTdata
        //#define clearEVENTdata
    #define createWEATHERdata           //uncomment to enable weather screen
    //#define clearWEATHERdata              //uncomment to disable weather screen
    #define createTHERMOSTATdata        //uncomment to enable thermostat screen
    //#define clearTHERMOSTATdata           //uncomment to disable thermostat screen
    #define createSENSORS               //uncomment to create fake sensors
        //#define clearSENSORS                  //uncomment to remove fake sensors
        
//***************************************************************************************************************************

    //ifdefs for showing testing data
    #ifdef createFAQ                    //runs scripting string commands for style, color, location
    uint8_t faq[] = 
                    "\x1b\x02\x00\x00\x60\x00"          //set cursor location 
                    /*"\x1b\x01\x00\x00"                  //style - Small, Normal
                    "\x1b\x00\xff\x00\x00"              //color - red
                    "sma"
                    "\x1b\x00\x00\x00\xff"              //color - blue
                    "ll no"
                    "\x1b\x00\x00\xff\x00"              //color - green
                    "rmal"
                    "\x1b\x00\xff\xff\xff"              //color - white
                    "\x1b\x01\x00\x01"                  //style - small, bold
                    "\nsmall bold"
                    "\x1b\x01\x01\x00"                  //style - medium normal    
                    "\nmedium normal"
                    "\x1b\x01\x01\x01"                  //style - medium bold
                    "\nmedium bold"
                    "\x1b\x01\x02\x00"                  //stlye - large nromal
                    "\nlarge normal"
                    "\x1b\x01\x02\x01"                  //style - large bold
                    "\nlarge bold"*/
                    
                    "\x1b\x2\xa0\x0\x70\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x0"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Arden Hills\n"
                    "\x1b\x9\x2"                  //center justify the text
                    "Welcomes\n"
                    "\x1b\x9\x2"                  //center justify the text
                    "UTCFS GSP\n"
                    "\x1b\x9\x2"                  //center justify the text
                    "leadership team"
                    
                    "\x1b\x0f\x6e"
                    
                    "\xff"
                    ;
                    
                    copyN(faq, nvData.faq, sizeof(nvData.faq));
    #endif
    #ifdef createDEALERcontact          //runs scripting string commands for style, justiifcation, location, logo display, display image
    uint8_t dealer[] =/*
                    "\x1b\x01\x00\x00"                  //style - Small, Normal
                    "\x1b\x00\xff\xff\xff"              //color - white
                    
                    "\x1b\x09\x01"                      //justification - right
                    "\x1b\x2\xd0\x00\x80\x00"           //set cursor location 
                    "right justified"

                    "\x1b\x09\x00"                      //justification - left
                    "\x1b\x2\xd0\x00\x90\x00"           //set cursor location 
                    "left justified"

                    "\x1b\x09\x02"                      //justification - center
                    "\x1b\x2\xd0\x00\xa0\x00"           //set cursor location 
                    "center justified"
                    */
                    "\x1b\x2\xa0\x0\x60\x0"       //set the text start position
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "\x1b\x09\x00"                      //justification - left
                    "  Need help?"
                    "\x1b\x2\x70\x0\x60\x0"       //set the text start position
                    "   Call:  (800) 555-1337"
                    "\x1b\x2\x70\x0\x70\x0"       //set the text start position
                    "Email:  help@contactus.com"
                    "\x1b\x2\x00\x0\x90\x0"       //set the text start position
                    "     or press a button below to be contacted"
                    "\x1b\x2\x00\x0\xa0\x0"       //set the text start position
                    "     by the end of the next business day"
                    
                    "\x1b\x05"
                    "\x1b\x06"
                    "\x1b\x07"
                    
                    //"\x1b\x03\x09\x00\x50\x00"          //logo display
                    
                    //"\x1b\x04\xff\x00\x50\x00\x83\x00"  //display image
                    "\xff"
                    ;
                    
                    copyN(dealer, nvData.dealer_contact_msg, sizeof(nvData.dealer_contact_msg));
//                    set_logo(1); //                    
    #endif
    #ifdef createADpage                 //runs scripting string commands for ad icons and ad text, will enable ad screens
    
    uint8_t txtIMG[] =  "\x1b\x0\xff\x40\x40"       //mix some green and blue with the red)
                        "Some Red..."           
                        "\x1b\x0\x40\xff\x40"
                        "Some Green..."
                        "\x1b\x0\x40\x40\xff"
                        "Some Blue..."
                        "\x1b\x0\xff\x40\xff"
                        "Some Purple..."
                        "\x1b\x0\xf0\xf0\xf0"
                        "Some White...";
    set_service_provider_msg(txtIMG, sizeof(txtIMG));
                                
                
    uint8_t ad00[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x00"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Entertainment Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is an entertainment special"
                    "\xff"
                    ;   
    uint8_t ad01[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x01"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Restaurant Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text            
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is a restaurantnt special"
                    "\xff";
#if 1  
    uint8_t ad02[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x02"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Automotive Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text            
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is an automotive special"
                    "\xff";
    uint8_t ad03[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x03"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Holiday Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text            
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is a holiday special"
                    "\xff";
    uint8_t ad04[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x04"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Vacation Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text            
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is a vacation special"
                    "\xff"; 
    uint8_t ad05[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x05"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Well-Being Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text            
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is a well-being special"
                    "\xff";
    uint8_t ad06[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x06"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Retail Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text            
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is a retail special"
                    "\xff";
    const uint8_t const ad07[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x07"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Online Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text    
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is an online special"
                    "\xff";
    uint8_t ad08[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x08"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Electronics Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text            
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is an electronics special"
                    "\xff";   
    const uint8_t const ad09[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x09"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Cell Phone Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text    
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is a cell phone special"
                    "\xff";
    const uint8_t const ad10[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x0a"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Home Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text    
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is a home special"
                    "\xff";
    const uint8_t const ad11[] =     
                    "\x1b\x08\xd5\x00\x55\x00\x0b"//place the icon
                    "\x1b\x2\xa0\x0\x95\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "\x1b\x0\x20\xff\x20"         //set the color:mostly green 
                    "Other Special"
                    "\x1b\x2\xa0\x0\xb0\x0"       //set the text start position
                    "\x1b\x9\x2"                  //center justify the text    
                    "\x1b\x1\x1\x0"               //set font:medium and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:white
                    "This is an other special"
                    "\xff";
#endif                    
    uint8_t ad12[] =                              //This must be set or no other ads will display  
                    "\x1b\x2\xa0\x0\x80\x0"       //set the text start position 
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x0"               //set font:large and normal
                    "\x1b\x0\xff\xff\xff"         //set the color:mostly green 
                    "Welcome To\n"
                    "\x1b\x9\x2"                  //center justify the text
                    "\x1b\x1\x2\x1"               //set font:large and bold
                    "UTCFS"
                    "\xff";
      
    set_ad_msg(0,(uint8_t*)ad00,sizeof(ad00));
    set_ad_msg(1,(uint8_t*)ad01,sizeof(ad01));
#if 1
    set_ad_msg(2,(uint8_t*)ad02,sizeof(ad02));
    set_ad_msg(3,(uint8_t*)ad03,sizeof(ad03));
    set_ad_msg(4,(uint8_t*)ad04,sizeof(ad04));
    set_ad_msg(5,(uint8_t*)ad05,sizeof(ad05));
    set_ad_msg(6,(uint8_t*)ad06,sizeof(ad06));
    set_ad_msg(7,(uint8_t*)ad07,sizeof(ad07));
    set_ad_msg(8,(uint8_t*)ad08,sizeof(ad08));
    set_ad_msg(9,(uint8_t*)ad09,sizeof(ad09));
    set_ad_msg(10,(uint8_t*)ad10,sizeof(ad10));
    set_ad_msg(11,(uint8_t*)ad11,sizeof(ad11));
#endif
    
    
    //set_ad_msg(12,(uint8_t*)"\n\n\n   cccccccc\xff", 19);  
    set_ad_msg(12,(uint8_t*)ad12,sizeof(ad12));             //ad12 must be set for other adds to display
    #endif
    #ifdef createDOORlocksANDlights     //adds lights and door locks screen
    nvData.light_data.light_name[0][0] = 1;      
    nvData.light_data.light_name[1][0] = 2;
    nvData.light_data.light_name[2][0] = 3;
    nvData.light_data.light_name[3][0] = 4;
    nvData.light_data.light_name[4][0] = 5;
    nvData.light_data.light_name[5][0] = 6;
    nvData.light_data.light_name[6][0] = 7;    
    nvData.light_data.light_name[7][0] = 8;    
    
    nvData.light_data.light_name[1][1] = 0xff;
    nvData.light_data.light_name[2][1] = 0xff;
    nvData.light_data.light_name[3][1] = 0xff;
    nvData.light_data.light_name[4][1] = 0xff;
    nvData.light_data.light_name[5][1] = 0xff;
    nvData.light_data.light_name[6][1] = 0xff;
    nvData.light_data.light_name[7][1] = 0xff;        
    
    nvData.light_data.enable_mask = 0b11111111;
    
    nvData.doorlock_data.doorlock_sensor[0] = 0;
    nvData.doorlock_data.doorlock_sensor[1] = 1;
    nvData.doorlock_data.doorlock_sensor[2] = 2;
    nvData.doorlock_data.doorlock_sensor[3] = 3;
    
    nvData.doorlock_data.enable_mask = 0b00001111;
    #endif
    #ifdef setCALIBRATIONvalues         //sets default calibration values
    nvData.cal.lh_offset = -40;
    nvData.cal.uh_offset = -40;
    nvData.cal.lv_offset = -40;
    nvData.cal.uv_offset = -40;
    nvData.cal.lh_factor = 5260;//5000;
    nvData.cal.uh_factor = 5160;//5000;
    nvData.cal.lv_factor = 4300;//4300;
    nvData.cal.uv_factor = 4300;//4300; 
    #endif
    #ifdef createEVENTdata              //creates fake data for the event screen
    union TwttsEvent tevent;
    
    
    for (i=0; i<30; i++)
    {
        tevent.tm_year = 123;
        tevent.tm_mon = 4;
        tevent.tm_mday = 5;
        tevent.tm_wday = 6;
        tevent.tm_hour = 7;
        tevent.noreport = 1;
        tevent.sensor = 43;
        tevent.tm_min = 22;
        tevent.user = 22;
        tevent.type = 0;
        
        store_event(&tevent);
    }
    
    #endif
    #ifdef createWEATHERdata            //create weather data
    vData.weather[0].hightemp= 127;
    vData.weather[0].lowtemp = -20;
    vData.weather[0].precip = 0;
    vData.weather[0].weathericon = 1;
    
    vData.weather[1].hightemp= 75;
    vData.weather[1].lowtemp = -90;
    vData.weather[1].precip = 10;
    vData.weather[1].weathericon = 1;
    
    vData.weather[2].hightemp=65;
    vData.weather[2].lowtemp = -80;
    vData.weather[2].precip = 20;
    vData.weather[2].weathericon = 2;
    
    vData.weather[3].hightemp=55;
    vData.weather[3].lowtemp = -70;
    vData.weather[3].precip = 30;
    vData.weather[3].weathericon = 3;
    
    vData.weather[4].hightemp = 45;
    vData.weather[4].lowtemp = -60;
    vData.weather[4].precip = 40;
    vData.weather[4].weathericon = 4;
    
    vData.weather[5].hightemp= 100;
    vData.weather[5].lowtemp = 70;
    vData.weather[5].precip = 108;
    vData.weather[5].weathericon = 5;                
    
    vData.weather[6].hightemp= 25;
    vData.weather[6].lowtemp = -40;
    vData.weather[6].precip = 60;
    vData.weather[6].weathericon = 6;
    build_tm_strings(&vData.last_weather_update_tm,NULL,0); 
    
    #endif
    #ifdef createTHERMOSTATdata         //create thermostat data
    vData.thermostat[0].tStatTemp = 55;
    vData.thermostat[0].tStatCoolingSetPoint = 60;
    vData.thermostat[0].tStatFanState = 1;
    vData.thermostat[0].tStatHeatingSetPoint = 55;
    vData.thermostat[0].tStatMode = 1;
    vData.thermostat[0].tStatSchedules = 0;
    vData.thermostat[0].tStatDegC = 0; 
    
    vData.thermostat[1].tStatTemp = 75;
    vData.thermostat[1].tStatCoolingSetPoint = 15;
    vData.thermostat[1].tStatFanState = 1;
    vData.thermostat[1].tStatHeatingSetPoint = 12;
    vData.thermostat[1].tStatMode = 1;
    vData.thermostat[1].tStatSchedules = 0;
    vData.thermostat[1].tStatDegC = 1;     
    
    
    vData.thermostat[2].tStatTemp = 138;
    vData.thermostat[2].tStatCoolingSetPoint = 90;
    vData.thermostat[2].tStatFanState = 1;
    vData.thermostat[2].tStatHeatingSetPoint = 80;
    vData.thermostat[2].tStatMode = 2;
    vData.thermostat[2].tStatSchedules = 1;
    vData.thermostat[2].tStatDegC = 0;
    #endif
    #ifdef createSENSORS                //create fake sensors
    
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[0],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[1],2);
    
    copyN((uint8_t*)"\x02\xff",nvData.sensor_text[2],2);    //doors
    copyN((uint8_t*)"\x04\xff",nvData.sensor_text[3],2);
    copyN((uint8_t*)"\x06\xff",nvData.sensor_text[4],2);
    
    copyN((uint8_t*)"\x03\xff",nvData.sensor_text[5],2);    //windows
    copyN((uint8_t*)"\x05\xff",nvData.sensor_text[6],2);
    copyN((uint8_t*)"\x07\xff",nvData.sensor_text[7],2); 
    copyN((uint8_t*)"\x09\xff",nvData.sensor_text[8],2); 
    copyN((uint8_t*)"\x0b\xff",nvData.sensor_text[9],2); 
    copyN((uint8_t*)"\x0d\xff",nvData.sensor_text[10],2); 
    copyN((uint8_t*)"\x0f\xff",nvData.sensor_text[11],2); 
    copyN((uint8_t*)"\x12\xff",nvData.sensor_text[12],2); 
    copyN((uint8_t*)"\x14\xff",nvData.sensor_text[13],2); 
    copyN((uint8_t*)"\x16\xff",nvData.sensor_text[14],2); 
    copyN((uint8_t*)"\x19\xff",nvData.sensor_text[15],2); 
    copyN((uint8_t*)"\x1b\xff",nvData.sensor_text[16],2); 
    copyN((uint8_t*)"\x1f\xff",nvData.sensor_text[17],2); 
    copyN((uint8_t*)"\x21\xff",nvData.sensor_text[18],2); 
    copyN((uint8_t*)"\x23\xff",nvData.sensor_text[19],2); 
    copyN((uint8_t*)"\x87\xff",nvData.sensor_text[20],2);
    
    copyN((uint8_t*)"\x08\xff",nvData.sensor_text[21],2);   //motions
    copyN((uint8_t*)"\x0a\xff",nvData.sensor_text[22],2); 
    copyN((uint8_t*)"\x0c\xff",nvData.sensor_text[23],2); 
    copyN((uint8_t*)"\x0e\xff",nvData.sensor_text[24],2);
    
    copyN((uint8_t*)"\x10\xff",nvData.sensor_text[25],2);   //property
    copyN((uint8_t*)"\x11\xff",nvData.sensor_text[26],2); 
    copyN((uint8_t*)"\x13\xff",nvData.sensor_text[27],2); 
    copyN((uint8_t*)"\x15\xff",nvData.sensor_text[28],2); 
    copyN((uint8_t*)"\x18\xff",nvData.sensor_text[29],2); 
    copyN((uint8_t*)"\x1a\xff",nvData.sensor_text[30],2); 
    copyN((uint8_t*)"\x1c\xff",nvData.sensor_text[31],2);    
    copyN((uint8_t*)"\x1d\xff",nvData.sensor_text[32],2);
    copyN((uint8_t*)"\x1e\xff",nvData.sensor_text[33],2);
    
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[34],2);         
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[35],2);                          
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[36],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[37],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[38],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[39],2);
    
    #if 1
        nvData.sensor_group[0]  = 0xff;
        nvData.sensor_group[1]  = 0xff;
        
        nvData.sensor_group[2]  = 0x0a; //D/W group
        nvData.sensor_group[3]  = 0x0a;
        nvData.sensor_group[4]  = 0x0a;
        nvData.sensor_group[5]  = 0x0a;
        nvData.sensor_group[6]  = 0x0a;
        nvData.sensor_group[7]  = 0x0a;
        nvData.sensor_group[8]  = 0x0a;
        nvData.sensor_group[9]  = 0x0a;
        nvData.sensor_group[10] = 0x0a;
        nvData.sensor_group[11] = 0x0a;
        nvData.sensor_group[12] = 0x0a;
        nvData.sensor_group[13] = 0x0a;
        nvData.sensor_group[14] = 0x0a;
        nvData.sensor_group[15] = 0x0a;
        nvData.sensor_group[16] = 0x0a;
        nvData.sensor_group[17] = 0x0a;
        nvData.sensor_group[18] = 0x0a;
        nvData.sensor_group[19] = 0x0a;
        nvData.sensor_group[20] = 0x0a;
        
        nvData.sensor_group[21] = 0x0f; //motions
        nvData.sensor_group[22] = 0x0f;
        nvData.sensor_group[23] = 0x0f;
        nvData.sensor_group[24] = 0x0f;
        
        nvData.sensor_group[25] = 0x03; //property
        nvData.sensor_group[26] = 0x03;
        nvData.sensor_group[27] = 0x03;
        nvData.sensor_group[28] = 0x03;
        nvData.sensor_group[29] = 0x03;
        nvData.sensor_group[30] = 0x03;
        nvData.sensor_group[31] = 0x03;
        nvData.sensor_group[32] = 0x03;
        nvData.sensor_group[33] = 0x03;
        nvData.sensor_group[34] = 0x03;
        
        nvData.sensor_group[35] = 0xff;
        nvData.sensor_group[36] = 0xff;
        nvData.sensor_group[37] = 0xff;
        nvData.sensor_group[38] = 0xff;
        nvData.sensor_group[39] = 0xff;
        
        //set_recheck_present_flags();
        
    #endif
    
    #if 1
        vData.motion_status[21].motion_min_tmr = 7;
        vData.motion_status[22].motion_min_tmr = 9;
        vData.motion_status[23].motion_min_tmr = 6;
        vData.motion_status[24].motion_min_tmr = 4;
    #endif
    
    for(i=0;i < 40;i++)
    {
        if(nvData.sensor_text[i][0] != 0xff)
            vData.CHNDAT[i].chn_bits._open=1;
    }    
    set_door_window_status();
    #endif
    
    //ifdefs for removing testing data
    #ifdef clearFAQ
    uint8_t faq[] = 
                    "\xff"
                    ;
                    
                    copyN(faq, nvData.faq, sizeof(nvData.faq));
    #endif
    #ifdef clearDEALERcontact
    uint8_t dealer[] =
                    "\xff"
                    ;
                    
                    copyN(dealer, nvData.dealer_contact_msg, sizeof(nvData.dealer_contact_msg));
    #endif
    #ifdef clearADpage                  //disables ad screens
    
    uint8_t effs[2] = {0xff,0xff};  //I dont think this is really necessary
    
    set_ad_msg(0,(uint8_t*)effs,0);
    set_ad_msg(1,(uint8_t*)effs,0);
    set_ad_msg(2,(uint8_t*)effs,0);
    set_ad_msg(3,(uint8_t*)effs,0);
    set_ad_msg(4,(uint8_t*)effs,0);
    set_ad_msg(5,(uint8_t*)effs,0);
    set_ad_msg(6,(uint8_t*)effs,0);
    set_ad_msg(7,(uint8_t*)effs,0);
    set_ad_msg(8,(uint8_t*)effs,0);
    set_ad_msg(9,(uint8_t*)effs,0);
    set_ad_msg(10,(uint8_t*)effs,0);
    set_ad_msg(11,(uint8_t*)effs,0);
    set_ad_msg(12,(uint8_t*)effs,0); 
    #endif
    #ifdef clearDOORlocksANDlights      //removes light and door locks screens
    nvData.light_data.light_name[0][0] = 0xff;      
    nvData.light_data.light_name[1][0] = 0xff;
    nvData.light_data.light_name[2][0] = 0xff;
    nvData.light_data.light_name[3][0] = 0xff;
    nvData.light_data.light_name[4][0] = 0xff;
    nvData.light_data.light_name[5][0] = 0xff;
    nvData.light_data.light_name[6][0] = 0xff;    
    nvData.light_data.light_name[7][0] = 0xff;    
    
    nvData.light_data.light_name[1][1] = 0xff;
    nvData.light_data.light_name[2][1] = 0xff;
    nvData.light_data.light_name[3][1] = 0xff;
    nvData.light_data.light_name[4][1] = 0xff;
    nvData.light_data.light_name[5][1] = 0xff;
    nvData.light_data.light_name[6][1] = 0xff;
    nvData.light_data.light_name[7][1] = 0xff;        
    
    nvData.light_data.enable_mask = 0b00000000;
    
    nvData.doorlock_data.doorlock_sensor[0] = 0xff;
    nvData.doorlock_data.doorlock_sensor[1] = 0xff;
    nvData.doorlock_data.doorlock_sensor[2] = 0xff;
    nvData.doorlock_data.doorlock_sensor[3] = 0xff;
    
    nvData.doorlock_data.enable_mask = 0b00000000;
    #endif
    #ifdef clearWEATHERdata             //removes weather data
    vData.weather[0].weathericon = 0x80;
    vData.weather[1].weathericon = 0x80;
    vData.weather[2].weathericon = 0x80;
    vData.weather[3].weathericon = 0x80;
    vData.weather[4].weathericon = 0x80;
    vData.weather[5].weathericon = 0x80;
    #endif
    #ifdef clearTHERMOSTATdata          //removes thermostat data
    vData.thermostat[0].tStatTemp = 0xff;
    vData.thermostat[1].tStatTemp = 0xff;
    vData.thermostat[2].tStatTemp = 0xff;
    #endif
    #ifdef clearSENSORS                 //removes fake sensors
    
    copyN((uint8_t*)"\xff\xff\xff\xff",nvData.sensor_text[11],4);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[12],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[13],2);         
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[14],2);                          
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[15],2);                            
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[16],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[17],2);    
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[18],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[19],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[20],2);         
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[21],2);                      
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[22],2);                            
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[23],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[24],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[25],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[26],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[27],2);         
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[28],2);                          
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[29],2);                            
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[30],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[31],2);    
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[32],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[33],2);
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[34],2);         
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[35],2);                          
    copyN((uint8_t*)"\xff\xff",nvData.sensor_text[36],2);
    #endif

#endif    
    
    nvBackupStatus = NVBACKUP_START;

#endif   
}

/**
 *
 *  Check whether nvData CRC is ok
 *
 */
void nvIntegrity(void)
{
    //TBD UT Need to check first whether we are in the process of backing up data
    // Calculate CRC over nvData except last two bytes, which is the CRC
//    if (calculatedCRC() != nvData.CRC)
    {
        // CRC is not ok, must restore data from flash by causing watchdog restart
        // watchdog may not be enabled during development
        //while(1); TBD UT
        __asm__ volatile ("reset");
    }
}

/**
 *
 *  Calculate CRC over .nvdata section except last two bytes (stored CRC) and return 16-bit value
 *
 */
//uint16_t calculatedCRC(void)
//{
//    // calculate 16-bit CRC over nvData
//    uint16_t *temp;
//    uint16_t len;           // length of data in bytes
//	uint16_t idx;
//
////    CRCCON = 0x0000F;       // length of polynomial - 1
////    CRCXOR = 0x1021;        // generator polynomial
////    CRCWDAT = 0x0000;       // initialize with 0
////
////    temp = (uint16_t *) &nvData;
////    len = sizeof(nvData)/2 - 1;
////    for (idx = 0; idx < len; idx++)
////    {
////        CRCDAT = *temp++;
////        if (CRCCONbits.CRCFUL == 1)
////        {
////            CRCCONbits.CRCGO = 1;
////            while (CRCCONbits.CRCMPT != 1);
////            Nop();
////            Nop();
////            Nop();
////            Nop();
////            Nop();
////            CRCCONbits.CRCGO = 0;
////        }
////    }
////
////    if (CRCCONbits.CRCGO != 1)
////        CRCCONbits.CRCGO = 1;
////
////    CRCDAT = 0x0000;
////
////    while (CRCCONbits.CRCMPT != 1);
////    Nop();
////    Nop();
////    Nop();
////    Nop();
////    Nop();
////    CRCCONbits.CRCGO = 0;
////    Nop();
//
//    return CRCWDAT;
//}
//    
/**
 *
 * set the arming level
 *
 */
void set_arming_lvl(enum Arm_State level)
{
    vData.panel_status.arming_lvl = level;
}

/**
 *
 * get the arming level
 *
 */
enum Arm_State get_arming_lvl(void)
{
    return vData.panel_status.arming_lvl;
}


/**
 *
 *   Set Chime on or off
 *
 */
void set_chime(uint8_t value)
{
    struct PANELDATA panelData;
    
    panelData.inputDataLen = 1;
    panelData.data.byte = value;
    setPanelData(&panelData, tChimeEnable);
}

/**
 *
 *   Get Chime
 *   returns: 
 *              
 */
//uint8_t get_chime(void)
//{
//    struct PANELDATA panelData;    
    
//    panelData.inputDataLen = 1;
//    getPanelData(tChimeEnable, &panelData);
//    return panelData.data.byte;
//}

/**
 *
 *   Set Motion Chime on or off
 *
 */
void set_motionChime(uint8_t value)
{
    struct PANELDATA panelData;
    
    panelData.inputDataLen = 1;
    panelData.data.byte = value;
    setPanelData(&panelData, tMotionChimeEnable);
}

/**
 *
 *   Get Motion Chime
 *
 */
//uint8_t get_motionChime(void)
//{
//    struct PANELDATA panelData;    
//    
//    panelData.inputDataLen = 1;
//    getPanelData(tMotionChimeEnable, &panelData);
//    return panelData.data.byte;
//}

/**
 *
 *  set the dealer logo number
 *
 */ 
//void set_logo(uint8_t logo_index)
//{
//    if(logo_ok(get_logo_address(logo_index)))
//    {
//        nvData.logo_index = logo_index;
//    }       
//    else
//    {
//        nvData.logo_index = NO_LOGO;
//    }
//    vData.updateLogoButton = 1;
//    nvBackupStatus = NVBACKUP_START;
//}     
    
/**
 *
 *   Get Default Screen 0=blank, 1=main
 *
 */
uint8_t get_default_screen(void)
{
    return nvData.default_screen;
}
    
/**
 *
 *   Set Brightness value
 *
 */
void set_brightness(uint8_t value)
{
    #if 0
    struct PANELDATA panelData;
    panelData.inputDataLen = 1;
    panelData.data.byte = value;
    setPanelData(&panelData, tBrightness);
    #endif
    nvData.brightness = value;
    nvBackupStatus = NVBACKUP_START;
}

/**
 *
 *  Get Brightness value
 *
 */
uint8_t get_brightness(void)
{    
    extern struct NVDATA nvData;
    return nvData.brightness;
}

/**
 *
 *   Get/increment/decrement the audio value
 *
 */
uint8_t get_audio_volume(enum SCROLL_DIRECTION control)
{
    struct PANELDATA panelData;    
    
    panelData.inputDataLen = 1;
    panelData.data.byte = control;
    getPanelData(tSpeakerVolumeAdj, &panelData);    
    return panelData.data.byte;
}

/**
 *
 *   Get beep value
 *
 */
uint8_t get_beep_volume(enum SCROLL_DIRECTION control)
{
    struct PANELDATA panelData;    
    
    panelData.inputDataLen = 1;
    panelData.data.byte = control;
    getPanelData(tStatusBeepVolume, &panelData);    
    return panelData.data.byte;  
}
     
/**
 *
 * Set the Default Screen variable
 *
 */
//void set_default_screen(enum Keypress_Values value)
//{
//    switch(value)
//    {
//        case key_scn_blank:
//            nvData.default_screen = FALSE;
//            break;
//        case key_scn_main:
//            nvData.default_screen = TRUE;
//            break;
//    }
//    nvBackupStatus = NVBACKUP_START;
//}
 
// Get the AC fault status
uint8_t get_ac_status(void)
{
    return vData.panel_status.ac_lost;
}

// Get the Battery fault status (panel battery)
uint8_t get_batt_status(void)
{
    return vData.panel_status.battery_low;
}
             
/**
 *
 * Set the panel status
 *
 */
void set_panel_status( struct SYS_STATUS *panel_status)
{
    extern struct VDATA vData;
    if(panel_status)
    {
        if( panel_status->arming_lvl != vData.panel_status.arming_lvl)
        {
#if 0 //KGS removed audio.c
            audio_init_arming_beep();
#endif
            if( panel_status->arming_lvl <= disarmed)        //clear delays when panel is disarmed
            {
                clear_delay_info(); 
            }      
        }    
        vData.panel_status = *panel_status;
        if( vData.panel_status.byte3_designator ==0)
        {
            if( vData.misc_panel.interactive_services != vData.panel_status.byte3.bitS.interactive_services)
            {
                vData.updateProviderMsg = 1;                    //update the service message whenever interative message changes
            }
            vData.misc_panel = vData.panel_status.byte3.bitS;
        }
    }
}
  
/***************************************************************************
 * Thermostat set function suite
 ***************************************************************************/      
/**
 *
 * Set the mode of the specified thermostat
 *
 */
//void set_tstat_mode(uint8_t active_tstat, enum TSTAT_MODE mode)
//{
//    vData.thermostat[active_tstat].tStatMode = mode;       
//}    

/**
 *
 * set the what the current temperature for the specified
 * thermostat.
 *
 */
//void set_tstat_temp(uint8_t active_tstat, uint8_t temperature)
//{
//    vData.thermostat[active_tstat].tStatTemp = temperature;    
//}    

/**
 *
 * Set the fan state for the specified thermostate
 *
 */
//void set_tstat_fan(uint8_t active_tstat, enum FAN_STATE fan_state)
//{
////    vData.thermostat[active_tstat].tStatFanState = fan_state;
//}

/**
 *
 *  set celsius or farenheit
 *
 */
//void set_temp_scale(uint8_t active_tstat, uint8_t celsius)
//{
//    vData.thermostat[active_tstat].tStatDegC = celsius;
//}

/**
 *
 * set the specified thermostat's coolingpoint
 *
 */
//void set_tstat_cooling_pt(uint8_t active_tstat, uint8_t setpoint)
//{
//    uint8_t min_temp, max_temp;
//    
//    if ( vData.thermostat[active_tstat].tStatDegC ) //true => using celsius scale
//    {
////        min_temp = MIN_COOL_SP_CEL;    
//        max_temp = MAX_COOL_SP_CEL;
//    }
//    else
//    {  
//        min_temp = MIN_COOL_SP_F;
//        max_temp = MAX_COOL_SP_F;
//    }
//    if(setpoint >= min_temp && setpoint <= max_temp)
//    {
//        vData.thermostat[active_tstat].tStatCoolingSetPoint = setpoint;
//    }
//}    

/**
 *
 * set the specified thermostat's heating setpoint
 *
 */
//void set_tstat_heating_pt(uint8_t active_tstat, uint8_t setpoint)
//{
//    uint8_t min_temp, max_temp;
//    
////    if ( vData.thermostat[active_tstat].tStatDegC )  //true => using celsius scale
//    {
//        min_temp = MIN_HEAT_SP_CEL;    
//        max_temp = MAX_HEAT_SP_CEL;
//    }
//    else
//    {  
//        min_temp = MIN_HEAT_SP_F;
//        max_temp = MAX_HEAT_SP_F;
//    }
//    if(setpoint >= min_temp && setpoint <= max_temp)
//    {
////        vData.thermostat[active_tstat].tStatHeatingSetPoint = setpoint;
//    }
//}

/**
 *
 *  set the specified thermostat's schedule enable
 *
 */
void set_tstat_schedule(uint8_t active_tstat, uint8_t enable_sched)
{
//    vData.thermostat[active_tstat].tStatSchedules = enable_sched;           
}    

/**
 * Change the state of the AC trouble flag.
 * @param state 1 if AC lost, 0 if AC okay
 */
void set_ac_trouble(uint8_t state)
{
    vData.ac_trouble = state;               // touchscreen ac state
}

/**
 * Change the state of the battery trouble flag.
 * @param state 1 if low battery, 0 if battery okay
 */
void set_battery_trouble(uint8_t state)
{
    vData.battery_trouble = state;           // touchscreen battery state

}

void check_name_token_range(uint8_t *name_tokens, uint8_t length)
{
    uint8_t i, ii;
    
    for (i=0; i<length; i++)
    {
        if (((*(name_tokens+i)) > (NUMBER_OF_ZNTOKENS-1))&&(*(name_tokens+i)!=0xFF))
        {
            for (ii=i; ii<(((i/MAXNAMETOKENS)+1)*MAXNAMETOKENS) ; ii++)
            {
                *(name_tokens+ii)=*(name_tokens+ii+1);
                if ((*(name_tokens+ii)) == 0xFF)
                    break;
            }
        }
    }   
    return;
}

/**
 *
 * store the newly received dealer contact information message
 *
 */
//void set_dlr_contact_msg(uint8_t src[], uint8_t len)
//{
//  
//    if(len < sizeof(nvData.dealer_contact_msg))
//    {
//        memFill(nvData.dealer_contact_msg,0,sizeof(nvData.dealer_contact_msg));
//        copyN(src,nvData.dealer_contact_msg,len);
//        nvData.dealer_contact_msg[len] = 0;
//        nvBackupStatus = NVBACKUP_START;
//    }        
//}


#define JPEG_STREAM_SN_NUM_SIZE         1
#define JPEG_STREAM_SN_NUM_OFFSET       0
#define JPEG_STREAM_NAME_TOKENS_SIZE    16
#define JPEG_STREAM_NAME_TOKENS_OFFSET  (JPEG_STREAM_SN_NUM_SIZE)
#define JPEG_STREAM_TIME_STRUCT_SIZE    7
#define JPEG_STREAM_TIME_STRUCT_OFFSET (JPEG_STREAM_SN_NUM_SIZE  + JPEG_STREAM_NAME_TOKENS_SIZE)
#define JPEG_STREAM_CRC_SIZE            2
#define JPEG_STREAM_CRC_OFFSET   (JPEG_STREAM_SN_NUM_SIZE  + JPEG_STREAM_NAME_TOKENS_SIZE + JPEG_STREAM_TIME_STRUCT_SIZE)       
#define JPEG_STREAM_IMAGE_OFFSET (JPEG_STREAM_SN_NUM_SIZE + JPEG_STREAM_NAME_TOKENS_SIZE + JPEG_STREAM_TIME_STRUCT_SIZE + JPEG_STREAM_CRC_SIZE) 
/**
 *
 *  Receive stream data for stream text 132 bytes at a time. When all blocks are received 
 *  store the data as defined by the stream id. 
 *
 */
void buffer_stream(struct STREAM_DATA *stream, enum Stream_Id streamN)
{ 
    extern struct VDATA vData;
    extern struct NVDATA nvData;
    uint8_t i;
    uint16_t logo_i;
    uint8_t jpegIndex;
    uint8_t dataByte;
    
    uint32_t dst,src;
   
    //check if to many blocks
    if(stream->block_number > MAX_BLOCK_COUNT)
    {
        return;      //too many blocks
    } 
    switch(stream->status)
    {
        case stream_start:     
        case stream_inprocess:
        case stream_done:            
            if (stream->block_number == 0 || stream->status == stream_start)
            {
//                sFlashEraseSectors(getScratchSpaceAddress(), FLASH_SECTOR_SIZE*8); //erase flash scratch space prior to use (erase first 6 sectors of the scratch space).                
            }
//            dst = getScratchSpaceAddress();                 //get address of temporary storage space
            dst += (stream->block_number*STREAM_BLOCK_SZ);
            flash_write(dst, (unsigned char*)stream->pData, STREAM_BLOCK_SZ);                           
            if(stream->status == stream_done)               //if stream status = done store the data
            {
//                src = getScratchSpaceAddress();             //get address of temporary storage space
                switch(stream->stream_id)
                {
                    case photograph_end:
//                        jpegIndex = getNewestPhotoIndex();  //there is a byte in flash that tells us which photograph is the newest
//                        jpegIndex++;                        //the next one must be the oldest, so we will overwrite it
//                        if (jpegIndex > 9)                  //cannot go out of range
//                        {    
//                            jpegIndex = 0;                  
//                        }
//                        writeJPEGflashFromStream(jpegIndex, src+JPEG_STREAM_IMAGE_OFFSET, dst+STREAM_BLOCK_SZ); //copy the actual image
//                        flash_read(src+JPEG_STREAM_SN_NUM_OFFSET, &dataByte, 1);                        
//                        setJPEGsensorNumber(jpegIndex, &dataByte);    //writing jpeg info => 1 byte for sensor number 16 bytes of name tokens
//                        setJPEGsensorTokens(jpegIndex, src+JPEG_STREAM_NAME_TOKENS_OFFSET);
//                        setJPEGtimeStampCurrentTime(jpegIndex, src+JPEG_STREAM_TIME_STRUCT_OFFSET); //stamp the system time to the jpeg
//                        setNewestPhotoIndex(jpegIndex);         //update which photo is the newest
                        break;
                    case sensor_group:
//                        set_sensor_groups(src);
                        break;
                    case company_logo1:
                    case company_logo2:
                        logo_i = DFI_COMPANY_LOGO1;
                        if (stream->stream_id == company_logo2)
                        {
                            logo_i = DFI_COMPANY_LOGO2;                        
                        }      
                        //src = get_image_address(DFI_SCRATCH_SPACE);  //get address of temporary storage space
//                        src = getScratchSpaceAddress();  //get address of temporary storage space
//                        dst = get_image_address(logo_i);  
                        // if logo image is compressed (RLE), then decompress during transfer from scratch area to logo area
                        // we don't know exact size of compressed image (junk data at end of last block)
                        // We do know the final size of the decompressed image (width*height*2 + 6)
//                        RLE_decode_and_store(src,dst,1+(stream->block_number));
//                        for(i=0;get_logo_address(i) != logo_i/* && i < NUMBER_OF_LOGOS*/;i++);
                        //For now the logo index is always set to the downloaded image index.
                        //KGS TBD In the next cut make it possible to download an image w/o setting the logo index.
                        //To make the above possible it will be required that downloaded images must be verified 
                        //before the logo index can be set to display such an image. 
                        //
//                        set_logo(i);            //set the logo index
                        break;     
                    default:
                        if(stream->stream_id <= all_sensor_text)
                        {
//                            set_sensor_text(src,streamN);
                        }                        
                }
            }    
            break;
    }
}

/**
 *
 *  set the status for the passed in SensorN
 *
 */
void set_channel_status(uint8_t sensorN, uint8_t status)
{
    vData.CHNDAT[sensorN].chnByte = status;
}

/**
 *
 * get calibration screen data
 *
 */
struct CAL_SETTINGS get_calibration_settings(void)
{
    return nvData.cal;
}

/**
 *
 * set calibration screen data
 *
 */
void set_calibration_settings(struct CAL_SETTINGS cal)
{
    nvData.cal = cal;
    nvBackupStatus = NVBACKUP_START;
}

/**
 *
 * Initialize sensorque
 *
 */
void InitSensorQue(void)
{
    uint8_t x;
    
    for(x = 0; x < SENSOR_BUFFER_SIZE; x++)
    {
        sensorque[x].sensor_index = 0xFF;
    }    
}
/**
 *
 *   Get panel firmware version value
 *
 */
void get_panel_firmware_version(void)
{
    struct PANELDATA panelData;

    panelData.inputDataLen = 1;
    getPanelData(tGetPanelRevison, &panelData);
    nvData.panel_firmware_ver=panelData.data.array[0];
    nvData.panel_patchNumber=panelData.data.array[1];
}

/**
 *
 * Send pass through message indicating display version number
 *
 * input:
 *  control = 0 => send when 24 hour timer expires;
 *          = 1 => send immediately
 *  
 *  assumptions:
 *      - function called hourly
 *      - min timer is cleared whenever packet is transmitted
 */
void sendDisplayVersion(uint8_t control)
{
    #define DISPLAY_VERSION_RESEND_TM_INTERVAL 120 //in minutes
    struct Message_Data eventMsg;
    static uint16_t minTmr = 0;
    
    if (control == 1 || minTmr == 0)
    {
        eventMsg.message_info.uplink_pkt.ulink_pkt_type = type_pass_through; 
        eventMsg.message_info.uplink_pkt.uplink_pkts.pass_through_uplink.pass_through_byte0 = BYTE0_DISPLAY_VERSION_HDR;
        eventMsg.message_info.uplink_pkt.uplink_pkts.pass_through_uplink.pass_through_byte1 = LBOARDFIRMWAREVERSION; 
        eventMsg.message_info.uplink_pkt.uplink_pkts.pass_through_uplink.pass_through_byte2 = LBOARDDEGVERSION;           //not used
        pnlComm_put(&eventMsg);
        minTmr = DISPLAY_VERSION_RESEND_TM_INTERVAL;
    }
    minTmr--;           //Note _24HourTmr is always none-zero before -- decrement (see above code logic))
}