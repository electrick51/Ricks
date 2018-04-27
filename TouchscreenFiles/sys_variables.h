#ifndef SYS_VARIABLES_H
#define SYS_VARIABLES_H

#include "dlpktdef.h"
#include "queue.h"

#include "sensor.h"

#include "rtcc.h"


// removed GMKX #include "rftrx.h"
#define QC375_FIX           //after keypad is used remove it from the screen stack-up
//#define QC376_FIX         //alarm time stamp remain constant whan alarm screen in re-netered. Not fixed for 2012
#define QC382_FIX           //patch number now displays correctly on the touch screen
#define QC386_FIX           //direct pass screen updated if arming level screen changes
//#define QC390_FIX         //2028 overflow for event buffer display. not fixed for 2012 1.5 release.
#define QC392_FIX           //date/time menu will time out
#define QC393_FIX           //require data/time to be access code protected
#define QC394_FIX           //light screen control fix (interactive services)

#if 0
/***********************************
    USER NUMBER STUFF
 ***********************************/
#define PANEL_USER_NUMBER       (NUMBER_OF_ZONES+3)
#define DEALER_USER_NUMBER      (PANEL_USER_NUMBER+1)       //simon3 user number 28
#define INSTALLER_USER_NUMBER   (DEALER_USER_NUMBER+1)      //simon3 user 29
#define MASTER_USER_NUMBER      (DEALER_USER_NUMBER+2)      //simon3 user 30
#define USER1                   (DEALER_USER_NUMBER+3)      //simon3 user 31
#define USER2                   (DEALER_USER_NUMBER+4)      //simon3 user 32
#define USER3                   (DEALER_USER_NUMBER+5)      //simon3 user 33
#define USER4                   (DEALER_USER_NUMBER+6)	    //simon3 user 34
#define USER5                   (DEALER_USER_NUMBER+7)      //simon3 user 35
#define USER6                   (DEALER_USER_NUMBER+8)
#define USER7                   (DEALER_USER_NUMBER+9)
#define USER8                   (DEALER_USER_NUMBER+10)
#define DURESS_USER_NUMBER      (DEALER_USER_NUMBER+11)
#endif

#define SCRATCH_BUFFER_SZ 150
#define DEALER_MSG_SZ 200           //number of bytes in dealer message


#define ImageOne 0 //KGS remove

#define SENSOR_BUFFER_SIZE 12


enum Arm_State {  
                    subdisarmed =0,   //must be 0 - subdisarmed?
                    disarmed =1,      //must be 1
                    doors=2,
                    motions=3,
                    doors_motions=4
               };

enum SCROLL_DIRECTION {eNeutral=0,eUp, eDwn, eMax, eMin};

// Functions
//******************************************************************
void set_arming_lvl(enum Arm_State level);
//void sys_variables_init(void);
uint16_t calculatedCRC(void);
void nvCopyToFlash(void);
void nvRestoreFromFlash(void);
void mfgRestoreFromFlash(void);
void nvDefault(void);
void nvBackupChk(void);
void nvIntegrity(void);
void xInit(void);
void vInit(void);
//void set_default_screen(enum Keypress_Values value);
void set_brightness(uint8_t value);
uint8_t get_brightness(void);
//void set_chime(BOOL value);
//BOOL get_chime(void);
//void set_motionChime(BOOL value);
//BOOL get_motionChime(void);
//void set_motionChime(BOOL value);
//BOOL get_motionChime(void);
// panel status
void set_panel_status( struct SYS_STATUS *panel_status);// , struct SYS_STATUS_2 *panel_status2);
//thermostat protos
//void set_tstat_set_pt(uint8_t tstat, enum TEMP_UP_DOWN up_down);
void set_tstat_temp(uint8_t tstat, uint8_t temperature);
//void set_tstat_mode(uint8_t tstat, enum TSTAT_MODE mode);
//void set_tstat_fan(uint8_t tstat, enum FAN_STATE fan_state);
void set_temp_scale(uint8_t tstat, uint8_t celsius);
void set_tstat_cooling_pt(uint8_t tstat, uint8_t setpoint);
void set_tstat_heating_pt(uint8_t tstat, uint8_t setpoint);
void set_tstat_schedule(uint8_t tstat,uint8_t tstat_run);
void set_ac_trouble(uint8_t state);
void set_battery_trouble(uint8_t state);
void buffer_stream(struct STREAM_DATA *stream, enum Stream_Id streamN);
void set_dlr_contact_msg(uint8_t src[], uint8_t len);
void set_channel_status(uint8_t sensorN, uint8_t status);
//void set_default_screen(BOOL value);
//BOOL get_default_screen(void);
uint8_t get_ac_status(void);
uint8_t get_batt_status(void);
void set_logo(uint8_t logo);
void get_panel_firmware_version(void);
uint8_t get_audio_volume(enum SCROLL_DIRECTION control);
void enroll_defaults(void);
void mfgCopyToFlash(void);
uint8_t get_beep_volume(enum SCROLL_DIRECTION control);

//void set_secure_ts(BOOL value);
//BOOL get_secure_ts(void);

void InitSensorQue(void);
void check_name_token_range(uint8_t *name, uint8_t length);
void default_msg_dwnloads(void);
void sendDisplayVersion(uint8_t control);


#define SIMONXTVER13 0x13
#define SIMONXTVER14 0x14

#define TWTTSVER10 0x10
#define TWTTSVER11 0x11

#define LBOARDFIRMWAREVERSION 0x50
#define LBOARDDEGVERSION 'F'  //set define to 0xff for production release

//*******************************************************************

enum NVBACKUPSTATE
{
    NVBACKUP_IDLE,
    NVBACKUP_START,
    NVBACKUP_TIMING
};

struct CAL_SETTINGS     // touchscreen calibration settings stored in nvData
{
   int8_t lh_offset;
   int8_t uh_offset;
   int8_t lv_offset;
   int8_t uv_offset;
   uint16_t lh_factor;
   uint16_t uh_factor;
   uint16_t lv_factor;
   uint16_t uv_factor;
};

void set_calibration_settings(struct CAL_SETTINGS cal);
struct CAL_SETTINGS get_calibration_settings(void);
enum Arm_State get_arming_lvl(void);



struct NVDATA
{
    /*
        All non-volatile variables should have a default value assigned in nvDefault()
        All data in a non-volatile member structure which isn't really non-volatile should have a suffix _x
        All data with a _x suffix should be initialized after a reset in function xInit()

        This structure will be instantiated once with a variable called nvData (struct NVDATA nvData),
        so that each variable in this structure will be accessed using something like nvData.var1.  nvData is
        located in a special section of RAM called .nvdata.

        Every member of this structure must have an associated set function.  This set function should be
        defined in sys_variables.c and must set nv

    */

    //add variables below as needed (also need set function, default value, reset value if _x)

    //uint8_t volume;                                 //voice volume
    //uint8_t beep_volume;                       // siren volume
    uint8_t brightness;                             //touch screen brightness
    uint8_t language;                               //not used yet
    uint16_t default_screen:1;                      //0 => main screen blank
    //uint16_t chime:1;                               // 0 = chime off : 1 = chime on
//    uint16_t secure_ts;                             // 0=> unsecure install, 1=> secure install
//    uint16_t lights:1;                              // 0 = lights off : 1 = lights on
    struct CAL_SETTINGS cal;                        // touchscreen calibration settings
    //uint8_t tpid1;   // moved to mfgData            // 20 bit touchscreen id
    //uint8_t tpid2;   // moved to mfgData            // sb factory set
    //uint8_t tpid3;   // moved to mfgData
    uint8_t teky2;                                  // encryption key-random at learn in
    uint8_t teky3;                                  // encryption key-random at learn in
    uint8_t teky4;    // moved to mfgData           // encryption key-random at learn in
    //uint8_t teky5;                                  // sb factory set
    uint8_t panelid1;                               // alarm panel id code
    uint8_t panelid2;
    uint8_t panelid3;
    //uint8_t trigValLo;
    //uint8_t trigValHi;
    //uint8_t enrolled;                               // 4 means enrolled
    uint8_t sensor_text[NUMBER_OF_ZONES][MAXNAMETOKENS];     //store the panel's sensor text data here
    uint8_t sensor_group[NUMBER_OF_ZONES];          //group #s for sensor
    struct LIGHT_DATA
    {
        uint8_t enable_mask;
//        uint8_t light_name[NUMBER_OF_LIGHTS][MAXNAMETOKENS];
    } light_data;
    struct DOORLOCK_DATA
    {
        uint8_t enable_mask;
//        uint8_t doorlock_sensor[NUMBER_OF_DOORLOCKS];
    } doorlock_data;
    uint8_t dealer_contact_msg[DEALER_MSG_SZ];
    uint8_t service_provider_msg[DEALER_MSG_SZ];
    uint8_t faq[DEALER_MSG_SZ];                     //frequently asked questions
    //uint8_t ad[DEALER_MSG_SZ];                    //advertisement
    //struct Thermostat thermostat[3];              //storage for 3 thermostats -- moved to vData
    //struct Weather_Data weather[7];               //0-sunday 6 = Saturday;
    //struct TM_DATE_STRINGS last_weather_update_tm;
    uint8_t logo_index;                             //index to logo table  0-3?
    uint8_t panel_firmware_ver;
    uint8_t panel_patchNumber;
    uint16_t CRC;   // CRC must be the last item in this structure
};

struct HR_MIN
{
    uint8_t tm_min;
    uint8_t tm_hour;
};

struct MOTION_STATUS
{
    uint8_t motion_min_tmr;
};

/**
 *
 * global volatile system variables
 *
 */
struct VDATA
{
    union _CHANNEL_STATUS CHNDAT[NUMBER_OF_ZONES+2];        //status of zones ;
    struct SYS_STATUS panel_status;
    struct MISC_PANEL misc_panel;
    struct MOTION_STATUS motion_status[NUMBER_OF_ZONES];
    struct DOOR_WINDOW_STATE
    {
        uint8_t doors_open:1;
        uint8_t windows_open:1;
        uint8_t other_open:1;                           //set => zone other than door or window is open
    }zn_state;
    uint8_t delay_tm;
    uint8_t entry_delay:1;                              //set => entry delay running
    uint8_t exit_delay:1;
    uint8_t motion_icon:1;                              //set => "motion icon" clear => "no motion icon"    
    uint8_t minute_tmr;                                 //increments once a minute
    uint8_t hour_tmr;                                   //increment once a hour
    uint8_t running_seconds;                            //increments every second
    uint8_t battery_voltage;                            //touch screen battery voltage  
    //struct SYS_STATUS_2
    uint8_t nu:1;                   // used to rf_dlink_ok
    uint8_t ac_trouble:1;           // AC fault
    uint8_t battery_trouble:1;      // Battery fault
    uint8_t battery_state_known:1;  //
    uint8_t updateProviderMsg:1;    //set => a update provider message
    uint8_t updateLogoButton:1;
    uint8_t asset_beep:1;           //set => sound one beep
    uint8_t lights:1;               // 0 = lights off : 1 = lights on
    uint8_t doors_present:1;        //set => door sensors are present in the system
    uint8_t windows_present:1;      //set => windows sensors are present in the system
    uint8_t motions_present:1;      //set => motions sensors are present in the system
    uint8_t property_present:1;     //set => property sensors are present in the system
    uint8_t panel_serial_channel_busy:1;
    uint8_t inProgramModeScreens:1; //set => program mode screens have been entered

    uint8_t clr_sequence_entered:1; //set => user has entered a clear sequence.
    // thermostat data moved from nvData
//    struct Thermostat thermostat[3];                    //storage for 3 thermostats
    uint16_t rangeTestTimer;
//    struct Weather_Data weather[7];                 //0-sunday 6 = Saturday;
    struct TM_DATE_STRINGS last_weather_update_tm;
    // moved from nvData
    uint8_t trigValLo;
    uint8_t trigValHi;
    RTCC rtc;
    uint8_t user_lvl;
    uint16_t password_tmr;
    uint16_t bannerTextTimer;
    uint16_t lcdDimTmr;                             //counts down in minutes
#ifdef QC376_FIX
    struct HR_MIN alarmTmStamp[NUMBER_OF_ZONES+2];        //holds time when a zone entered alarm....Note if touchscreen is reset this time will not be correct
#endif
};

struct MFGDATA
{
    uint16_t DisplayRev;  //0x1 => 480x272 display, 0xffff => 320x240 display
    uint8_t Nu1;
    uint8_t Nu2;
    uint8_t Nu3;
};

struct SENSORQUE
{
    struct TM_DATE_STRINGS sensor_event_tm;
    uint8_t new_event_flag;
    uint8_t sensor_index;
    
};
    
struct MOTIONLIST
{
    uint8_t zone;
    uint8_t time;
};
    

#endif
