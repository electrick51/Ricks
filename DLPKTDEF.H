#ifndef _DLPKTDEF_H
#define _DLPKTDEF_H

#include <time.h>
#include "EXACTINT.h"


#define STREAM_BLOCK_SZ 132

// Note: Number of vowels in this file's name > 0
//
// Also note: This file is used in the touchscreen and the panel projects.
//
// downlink packets
// downlink packets have
//
//  20 bit id
//  1 packet type code byte
//  3 data bytes
//  trigger or index byte
//  crc8
//
// must use packed bit structures to fit
// within the limited space in packet
//
// Downlink packets may be either broadcast or directed
// or pinged
// This applies to all packet types even though it may not
// make sense or be usable in every case

enum DlPktType {
SystemStatusPktCode=1,
SystemPingPktCode=2,
SetPingTimePktCode=3,
LearnPktCode=4,
TimePktCode=5,
DatePktCode=6,
SpeechTokenPktCode=7,
TextTokenPktCode=8,
TextSpeechTokenPktCode=9,
SounderPktCode=10,
WeatherForcastPktACode=11,
ZoneTokenPktCode=12,
DelayedSpeechTokenPktCode=13,
NxDelayPktCode=14,
NotUsed=15,
DealerMessageTextType5PktCode=16,
LogoBitMapPktCode=17,
TxtTokenDisplayPktCode=18,
TstatTemperatureDisplayPktACode=19,
RfEchoTestPktCode=20,
StartStreamBlockCode=21,
EndStreamBlockCode=22,
WeatherForcastPktBCode=23,
DealerMessageTextType6PktCode=24,
TstatTemperatureDisplayPktBCode=25,
ZoneStatusPktCode=26,
EvtPktACode = 27,                       //events are transmitted in 2 packets (a and b)
EvtPktBCode = 28,                       //events are transmitted in 2 packets (a and b)
ArmingStateCode = 29,
PassThroughPktCode = 30,
EnrollmentSuccessful = 31,
TwttsDownLinkTestCode = 32,
//DealerMessageTextFaqPktCode=33,       //frequently asked questions script string
DealerMessageAD1PktCode=34,             //advertisement code
DealerMessageAD2PktCode=35,
DealerMessageAD3PktCode=36,
DealerMessageAD4PktCode=37,
DealerMessageAD5PktCode=38,
DealerMessageAD6PktCode=39,
DealerMessageAD7PktCode=40,
DealerMessageAD8PktCode=41,
DealerMessageAD9PktCode=42,
DealerMessageAD10PktCode=43,
DealerMessageAD11PktCode=44,
DealerMessageAD12PktCode=45,
DealerMessageAD13PktCode=46,
DealerMessageAD14PktCode=47,
DealerMessageAD15PktCode=48,
DealerMessageAD16PktCode=49,

EquipMentListPktCode=50,                //send zwave equipment data
CodeResponsePktCode=51,                 //code entry response from panel
RunScriptCmdCode=52                     //run a script command 
};

enum ProgramParameter
{
	eAccountN = 1,						//read/write the Account Number
	eDownLoaderCode,					//read/write the downloader code
	ePasswordLen
};

#define MAXPANELARRAYSIZE 50

union INPUTDATATYPES
{
    uint8_t byte;
    uint8_t token;
    uint16_t uint;
    struct tm time;
    uint8_t array[MAXPANELARRAYSIZE];
};

#define eKeypadInput 0x01
#define eExtendedKeypadInput 0x02
#define eIncDecInput 0x04
#define eListInput 0x08
#define eRadioButtonListInputOneColumn 0x10
#define eRadioButtonListInputTwoColumn 0x20
#define ePhoneKeypadInput 0x40

enum DATATYPE
{
    eNoType=0,
    eUINT8_T,
    eUINT16_T,
    eARRAY,
    eSENSORTEXT,
    eCHECKBOX,
    eTIME,
    eSPECIAL,
    eTESTING,
    eVIRTUAL,
//  eINT8_T,
//  eINT16_T,
};

#define MAXSCRIPTSTRINGSIZE 150


enum PANELDATATOKEN
{
    tNoData = 0,

    //access codes
    tDealerCode,
    tInstallerCode,
    tMasterCode,
    tUserCode1,
    tUserCode2,
    tUserCode3,
    tUserCode4,
    tUserCode5,
    tUserCode6,
    tUserCode7, //10
    tUserCode8,
    tDuressCode,
    tCodeSize,
    
    //audio verification
    tAudioMode,
    tFireShutdown,
    tPanicTalk,
    tVOXreceiverGain,
    tVOXmicrophoneGain,
    tVOXgainRange,
    tManualMicrophoneGain,  //20
	tFireEnabledAvm,
    
    //communication modes
    tCommModesMenu,
    tPhone1Reports,   
    tPhone2Reports,
    tPhone3Reports,
    tPhone4Reports,
    
    //light control
    
    //phone numbers
    tPhoneNumber1,
    tPhoneNumber2,
    tPhoneNumber3,
    tPhoneNumber4,
    tDownloaderNumber,  //30
    
    //phone options
    tManualPhoneTest,
    tFailToCommunicate,
    tDTMFdialing,
    t300bpsEnabled,
    tRingHangRing,
    tDialerDelay,
    tCallWaitingCode,
    tLineCut,
    
    //reporting options
    tReportOptMenu,
    tOpeningReport,   
    tClosingReport,
    tForceArmedReport,  //40
    tACpowerFailureReport,
    tPanelLowBatteryReport,
    tSensorAlarmRestoralReport,
    t24hourSensorTamper,
    tSupervisoryTamperReport,
    tNoUsageReport,
    tSwingerShutdown,
    tProgrammingReport,
    tFireAlarmVerification,
    
    //reporting
    
    //programming
    
    //security
    tAccountNumber, //50
    tDownloaderCode,
    tPhoneLock,
    tAutoArm,
    tExitExtension,
    tSecureArming,
    tNoArmLowBatt,
    tQuickExit,
    tDownloaderEnable,
    tSupervisoryProtest,
    
    //sensors
	tSensorNumber,
	tSensorInventory,
    tSensorText, 
    tSensorEditText,//
    tSensorGroup,
    tSensorEditGroup,
	tSensorDel,					//delete sensor
    tSensorEnterLrnMode,        //enter sensor learn mode
	tSensorLeaveLrnMode,		//leave learn mode
    tSensorsLearned,            //0-39 bitfield indicated programmed status of each of 40 sensors
    tSensorInitForRead,         //Initialize sensor data structure enter sensor edit mode
    tSensorLrnStatus,           //query panel for learn status
    tSensorSaveNewSensor,       //move new sensor data into the panel's memory 
    tSensorSaveEditedSensor,    //save edited sensor 
    tSensorDirectBypass,        //direct bypass 
    tSensorStreamCheck,         //check for sensor streaming
    tSensor01,
    tSensor02,
    tSensor03,
    tSensor04,
    tSensor05,
    tSensor06,
    tSensor07,
    tSensor08,
    tSensor09,
    tSensor10,
    tSensor11,
    tSensor12,
    tSensor13,
    tSensor14,
    tSensor15,
    tSensor16,
    tSensor17,
    tSensor18,
    tSensor19,
    tSensor20,
    tSensor21,
    tSensor22,
    tSensor23,
    tSensor24,
    tSensor25,
    tSensor26,
    tSensor27,
    tSensor28,
    tSensor29,
    tSensor30,
    tSensor31,
    tSensor32,
    tSensor33,
    tSensor34,
    tSensor35,
    tSensor36,
    tSensor37,
    tSensor38,
    tSensor39,
    tSensor40,
    
    //siren options
    tPanelPiezoBeeps,
    tPanelVoice,
    tPanelPiezoAlarms,
    tTroubleBeeps,
    tVoiceChime,
    tStatusBeepVolume,
    tHardwiredSirenSupervision,
    tSpeakerVolumeAdj,
    tPanelSilentPolicePanic,
    tPanelTamperAlarm,
    tAlarmReportVerification,
    
    //system options
    tRFjamDetect,
    tDemoMode,
    tHW1function,
    tClockFormat24hour,
    
    //system tests
    tEnterSensorTest,
    tExitSensorTest,
    tGetSensorTestStatus,
    tCommTestEnter,
    tCommTestStatus,
    tDownloaderEnter,
    tDownloaderStatus,
    
    //timers
    tLatchkeyTime,   
    tEntryDelay,
    tExitDelay,
    tNoActivityTimeout,
    tAutoPhoneTest,
    tSupervisoryTimeHours,
    tSupervisoryTimeMinutes,
    tSupervisoryTimeAmPm,
    tAlarmCancelWindow,
    tRFtimeout,
    tFailToOpenTimeHours,
    tFailToOpenTimeMinutes,
    tFailToOpenTimeAmPm,
    tFailToCloseTimeHours,
    tFailToCloseTimeMinutes,
    tFailToCloseTimeAmPm,
    tSirenTimeout,
    tArmingLEDshutdown,
    tUnvacatedPremises,
    tSmokeSensorSupervision,
    
    //touchpad options
    tKeyfobNoDelay,
    tPanelPanicAlarms,
    tRemoteTouchpadArming,
    
    //programming screen (top level)
    tProgramMenu,
    tAccessCodesMenu,
    tSecurityMenu,
    tPhoneNumbersMenu,
    tPhoneOptionsMenu,
    tSensorsMenu,
    tReportingMenu,
    tTimersMenu,
    tTouchpadOptionsMenu,
    tSystemOptionsMenu,
    tSirenOptionsMenu,
    tAudioVerificationMenu,
    tLightControlMenu,
    tSystemTestsMenu,
    tInteractiveServicesMenu,
    
    tAccessCodeCheck,           //access code inquiry
    tGetPanelRevison,           //revision inquiry
    tTime,                      //set/read the time
    tDate,                      //set/read the date
    tMinute,
    tHour,
    tDay,
    tMonth,
    tYear,
    tAmPm,
    tUpdateDateTime,
    tBrightness,                //read/set the tft brightness (maps to contrast)
    tChimeEnable,               //enable/disable door chime
    tMotionChimeEnable,         //enable/disable motion chime
    tProgamModeTimer,           //read/set the program mode timer
    tInteractiveScreenScript,           //get screen script from the panel
    tInteractiveScreenKeyPress,        //send keypress to the panel
    tInteractiveServicesEntered,
    tInteractiveServicesExit,
    tCheckQuickExit,            //check if quick exit is available
    tPeekMemory,                //peek panel or touchscreen memory
    tPokeMemory,                //poke panel or touchscreen memory
};

struct PANELDATA
{
    uint16_t token;	//don't use enum to avoid size issue 1 byte-vs-2 bytes  enum PANELDATATOKEN token;
    uint8_t subIndex;    //used to further indentify token information. ex token = tSensor, subIndex used to indicate = sensor number
	uint16_t dataType; //don't use enum to avoid size issue 1 byte-vs-2 bytes enum DATATYPE dataType;
    uint8_t inputDataLen; //number of bytes placed into data
    uint8_t dataInfoLen;
    union INPUTDATATYPES data;
    uint8_t dataInfo[MAXSCRIPTSTRINGSIZE];
};

#define GLIMIT 143      // for twtts ver 1.0 zone stream token must be in
                        // (0,143) inclusive
//If enum assignments are changed make sure calls to (in simonXT code) dlTwtsImageUpdate()
//are adjusted... there are calls from assembly that use literals for the enumeration values:
// company_logo1=41, company_logo2=42, and faq_text = 43
enum Stream_Id
{
    sensor_1_text=0,
    sensor_2_text=1,
    sensor_3_text=2,
    sensor_4_text=3,
    sensor_5_text=4,
    sensor_6_text=5,
    sensor_7_text=6,
    sensor_8_text=7,
    sensor_9_text=8,
    sensor_10_text=9,
    sensor_11_text=10,
    sensor_12_text=11,
    sensor_13_text=12,
    sensor_14_text=13,
    sensor_15_text=14,
    sensor_16_text=15,
    sensor_17_text=16,
    sensor_18_text=17,
    sensor_19_text=18,
    sensor_20_text=19,
    sensor_21_text=20,
    sensor_22_text=21,
    sensor_23_text=22,
    sensor_24_text=23,
    sensor_25_text=24,
    sensor_26_text=25,
    sensor_27_text=26,
    sensor_28_text=27,
    sensor_29_text=28,
    sensor_30_text=29,
    sensor_31_text=30,
    sensor_32_text=31,
    sensor_33_text=32,
    sensor_34_text=33,
    sensor_35_text=34,
    sensor_36_text=35,
    sensor_37_text=36,
    sensor_38_text=37,
    sensor_39_text=38,
    sensor_40_text=39,
    all_sensor_text=40,
    company_logo1=41,
    company_logo2=42,
    faq_text=43,
    sensor_group=44,
    photographs = 45,
    photograph_next = 46,   // continue photo
    photograph_end  = 47    // ends photo

};

enum USERLVL        //access authority levels - for simonXT there are 4 levels of authorization
{
    dealer_lvl=0,
    installer_lvl=1,
    master_lvl=2,
    user_lvl=3,
    duress_lvl = 11,
    any_lvl=200,
    undefined_lvl=0xff      //no password user level entered            
};

enum ArmingStatus
{
    protesting=1,           //0-39 sensor, 40=>ac fail, 41=>battery fail, 42=>reporting failure, 43=>phone line fail
                            //44=>siren 1 fail, siren 2 fail
    waitingForCode=2,
    in_exit_delay = 3,
    panel_armed = 4,                             //=>panel is armed
    panel_disarmed = 5,
    in_entry_delay = 6,
    arming_idle = 7,
    in_exit_delay_directed  //this is a kluge the rf message structure should contain whether the received packed is: broadcast, directed, nsd information.. due to time constraints this
};                          // due to time constraints this is the safest solution

struct SYS_STATUS
{
      //byte 0
      uint8_t no_delay:1;
      uint8_t latchkey_alarm:1;
      uint8_t phoneLineFail:1;
      uint8_t no_activity_alarm:1;
      uint8_t quick_exit_on:1;
      uint8_t trouble_beeps:1;              //set => sound trouble beeps
      uint8_t panel_in_alarm:1;             //set => panel is in alarm
      uint8_t troubleStatusPresent:1;
      //byte 1
      uint8_t intrusion_alarm:1;
      uint8_t emergency_alarm:1;
      uint8_t fire_alarm:1;
      uint8_t siren_sounding:1;
      uint8_t code_required:1;                  //set => code required for requested operation
      uint8_t protesting:1;
      uint8_t SilentExitOn:1;
      uint8_t report_fail:1;
      //byte 2
      uint8_t arming_lvl:3;                     //panel arming level      
      uint8_t ac_lost:1;        // set => no AC      
      uint8_t battery_low:1;    // set => low battery
      uint8_t back_tamper:1;      
      uint8_t byte3_designator:2;          //1-entry delay, 2 exit delay; if 1 or 2 byte 3 contains the current delay time
      // byte 3                 // replaces trigger8
      union
      {
        struct MISC_PANEL
        {
            uint8_t siren1_fail:1;
            uint8_t siren2_fail:1;
            uint8_t emergency_button_enabled:1;         //set => sys has emergency button enabled
            uint8_t latchkey_en:1;                      //set => latchkey is enabled as an option?
            uint8_t interactive_services:1;
            uint8_t _24hr_format:1;                     //set => use 24 hour format when displaying time
            uint8_t day_month_order:1;                  //set => date displayed day/month clear => date displayed month/day
            //uint8_t enableEmergencyKeys:1;              //set => enable emergency keys
            uint8_t panelSecureArm:1;
        }bitS;    
        //uint8_t exit_or_entry_delay;
        //uint8_t protest_source;
      }byte3;      
};
/*
struct SYS_STATUS_2
{
    uint8_t siren1_fail:1;
    uint8_t siren2_fail:1;
    uint8_t emergency_button_enabled:1;         //set => sys has emergency button enabled
    uint8_t latchkey_en:1;                      //set => latchkey is enabled as an option?
    uint8_t nu4:4;
    uint16_t nu16:16;

};
*/
struct RF_Message_Pkt
{
	uint8_t type;                 //do not make this an (DlPktType) enum... causes a 16 bit value in renesas compiler even with -FCE option
//	struct SystemStatusData systemStatus;
//	uint8_t trigger8;  //KGS removed ... incorrect alignment with actual rf packet

	union RF_Message_Data
	{
		struct Generic
        {
   		    uint8_t byte1;
   		    uint8_t byte2;
   		    uint8_t byte3;
   		    uint8_t byte4;
        }generic_pkt;

        struct SystemStatusPkt
        {
		    struct SYS_STATUS systemStatus;
		}system_status_pkt;
/*
		struct SystemStatusPkt2
		{
		    struct SYS_STATUS_2 systemStatus2;
		    uint8_t trigger8;
		}system_status_pkt2;
*/
		struct systemPingPkt
        {
		    struct SYS_STATUS systemStatus;
		}system_ping_pkt;

/*
		struct SetPingPkt
        {
		    uint8_t notUsed1;
		    uint8_t notUsed2;
		    uint8_t timetoping;       // time to next ping
		    uint8_t trigger8;
		}set_ping_pkt;
*/
/*
		struct LearnPkt
        {
		    uint8_t notUsed1;
		    uint8_t notUsed2;
		    uint8_t notUset3;
		    uint8_t trigger8;
		}learn_pkt;
*/
		struct TimePkt
        {
		    uint8_t tm_hour;
		    uint8_t tm_min;
		    uint8_t tm_sec;
		    uint8_t trigger8;
		}time_pkt;

		struct DatePkt
        {
		    uint8_t notUsed:4;
		    uint8_t tm_mon:4;
		    uint8_t tm_mday:5;
		    uint8_t tm_wday:3;
		    uint8_t tm_year:8;
            uint8_t trigger8;
		}date_pkt;

		struct SpeechTokenPkt
        {
		    uint8_t tokena;          // 1 to 3 tokens, 1 extended token & 1 token
		    uint8_t tokenb;
		    uint8_t tokenc;
		    uint8_t trigger8;
		}speech_token_pkt;

		struct TextTokenPkt
        {
		    uint8_t tokena;          // 1 to 3 tokens, 1 extended token & 1 token
		    uint8_t tokenb;
		    uint8_t tokenc;
		    uint8_t trigger8;
		}text_token_pkt;

		struct TextSpeechTokenPkt
        {
		    uint8_t tokena;          // 1 to 3 tokens, 1 extended token & 1 token
		    uint8_t tokenb;
		    uint8_t tokenc;
		    uint8_t trigger8;
		}text_speech_token_pkt;


		struct DelayedSpeechTokenPkt {
		      uint8_t tokena;
		      uint8_t tokenb;
		      uint8_t tokenc;
		      uint8_t trigger8;
		}delayed_speech_token_pkt;

		struct SounderPkt {
		      uint8_t notUsed:3;
		      uint8_t sounderLevel:1;
		      uint8_t sounderWakeUp:1;
		      uint8_t sounderTimeSlice:1; // 0=125ms, 1=500ms
		      uint8_t sounderPeriod:2;    // 00=one shot, 01=1s, 10=2s, 11=4s
		      uint16_t sounderPattern;
		      uint8_t trigger8;
		}sounder_pkt;

		struct NxDelayPkt {
		      uint8_t delayType;         // exit,entry, quick exit
		      uint16_t delayTime;        // 0 to 65535 sec
		      uint8_t trigger8;
		}nx_delay_pkt;

		struct MessageTextPkt {
              uint8_t dbytea;
              uint8_t dbyteb;
              uint8_t dbytec;
              uint8_t dindex;           // index replaces trigger8
      }message_text_pkt;

		struct DealerMessageTextPkt {
              uint8_t dbytea;
              uint8_t dbyteb;
              uint8_t dbytec;
              uint8_t dindex;           // index of 0 indicates first packet, index is incremented by 3 for each subsequent packet (3 data bytes/packet)
              							// If index = 0, dbytea = number of bytes in message (includes CRC), dbyteb-dbytec = CRC as defined for downloader
        }dealer_message_text_pkt;

		struct LogoBitMapPkt {
		      uint8_t dbytea;
		      uint8_t dbyteb;
		      uint8_t dbytec;
		      uint8_t dindex;           // index replaces trigger8
		}logo_bitmap_pkt;

		struct WeatherForecastPktA {
		      uint8_t dayofweek;       // day 0-6-->Sun-Sat
		      uint8_t lowtemp;         // high
		      uint8_t hightemp;        // low
		      uint8_t trigger8;        // trigger8
		}weather_forecast_pkt_a;

		struct WeatherForecastPktB {
		      uint8_t dayofweek;
		      uint8_t weathericon;
		      uint8_t precip;
		      uint8_t trigger8;
		}weather_forecast_pkt_b;

		struct  ZoneStatusPkt
        {
		    uint8_t chime:6;                //0-tone, 1 voice, 2 chimeA, 3- chimeB otherwise off
		    uint8_t asset_detected:1;       //set=>asset mamagement zone
            uint8_t chime_motion_detected:1;//set => sensor state change => motion (used by twtts to time movement
		    uint8_t zoneNumber;             // zone #
		    union _CHANNEL_STATUS           //caution this stucture must not change. It must be the same as
            {                               //the structure CHANNEL_STATUS... to many header file issues with sc10 to use
                struct CHN_BITS             //CHANNEL_STATUS here... fix later?
                {
                    uint8_t lowBat:1;      // chandat bits
		            uint8_t alarmHistory:1;
		            uint8_t bypassed:1;
		            uint8_t supervisory:1;
                    uint8_t _open:1;        // (not restored)
		            uint8_t inAlarm:1;
		            uint8_t tampered:1;     // or passed sensor test
		            uint8_t activated:1;    // disarmed by delay
		        }chn_bits;
		        uint8_t chnByte;
            }chn_data;
            uint8_t trigger8;
		}zone_status_pkt;

		struct ZoneTextPkt
		{
		      uint8_t tokena;
		      uint8_t tokenb;
		      uint8_t tokenc;
		      uint8_t dindex;           // index replaces trigger8
		}zone_text_pkt;

		struct TxtTokenDisplayPkt
		{
		      uint16_t displayToken:9;
		      uint16_t tokena:7;
		      uint8_t timedLights:1;     //bit 0
		      uint8_t doorchime:1;       //bit 1
		      uint8_t motionchime:1;     //bit 2
		      uint8_t sensorActlitesB:1; //bit 3
		      uint8_t latchkeyB:1;       //bit 4
		      uint8_t subdisarm:1;       //bit 5
		      uint8_t armlvl:2;          //bit 6 & 7 2

		      uint8_t trigger8;
		}txt_token_display_pkt;

		struct TstatTemperatureDisplayPktA
        {
              uint8_t tStatTemp;			//current temperature (display verbatim)
              uint8_t tStatDegC:1;			//set => display in degrees C
              uint8_t notUsed;
              uint8_t trigger8;
        }tstat_temperature_display_pkt_a;

        struct TstatTemperatureDisplayPktB
        {
              uint8_t tStatCoolingSetPoint; //cooling setpoint (display verbatim)
              uint8_t tStatHeatingSetPoint; //heating setpoint (display verbatim)
              uint8_t tStatFanState:1;		//0-auto, 1-on
              uint8_t tStatMode:2;			//0-off, 1-cooling, 2-heating, 3-emergency heating
              uint8_t tStatNumber:2;		//identifies specific thermostat number	(0-2)
			  uint8_t tStatSchedules:1;		//0-schedules disabled, 1-schedules enabled
              uint8_t trigger8;
        }tstat_temperature_display_pkt_b;

		struct RfTestDlTestPkt{
		      uint8_t dbyte1;
		      uint8_t dbyte2;
		      uint8_t dbyte3;
		      uint8_t trigger8;
		}rf_test_dl_pkt;

		struct EventPktA        //events are transmitted in two packets
		{
			union TwttsEventA
			{
				//EvtPktACode = first 3 bytes of event
                struct
                {
                    uint16_t tm_year:7;		//7
		  		    uint16_t tm_mon:4;      //11
		   	        uint16_t tm_mday:5;		//16
				    uint8_t tm_wday:3;      //19
				    uint8_t tm_hour:5;		//24
		        }bitS;
			    uint8_t eventAll[3];
			}twtts_event_a;
			uint8_t trigger8;	        //rf trigger
		}event_pkt_a;

		struct EventPktB        //event are transmitted in two packets
		{
			union TwttsEventB
			{
				struct
				{
				    uint8_t noreport:1;	    //1  clear => reportable alarm (bit 3 of SimonXT)
				    uint8_t sensor:7;       //8  (used 8 bits for SimonXT)
				    uint16_t tm_min:6;      //14
		   	        uint16_t user:6;		//20  this allows for user 0-57
				    uint16_t type:4;        //24  (bit 7 and bit 0-2 on simonXT)
				}bitS;
				uint8_t eventAll[3];    //6  first 3 bytes == EvtPktACode. 2nd 3 bytes = EvtPktBCode
			}twtts_event_b;
			uint8_t trigger8;	        //rf trigger
		}event_pkt_b;

	    struct ArmingState
	    {
    	    union ARM_DATA
    	    {
                uint16_t exit_delay;
    	        struct
    	        {
        	        uint16_t sensorN:7;
        	        uint16_t fault_nbr:9;
        	    }protest;      
                  
            }arm_data;                            
    	    uint8_t arming_status:4;    //protesting=1, waitingForCode=2, in_exit_delay = 3, 4 =>in entry delay
    	    uint8_t nu4:4;              //
    	}arming_state;    		
		
		struct PassThroughPkt
	    {
		    uint8_t msg_type;
			uint8_t dbytea;
			uint8_t dbyteb;
			uint8_t trigger8;
    	}pass_through_pkt;

        struct CodeResponsePkt
        {
		    uint8_t usernumber;
		    uint8_t nu1;
		    uint8_t nu2;
		    uint8_t trigger8;
		}code_response_pkt;

        
//
//		msg_type: 1=logo select 
//		dbytea: logo ID
//		dbyteb: n/a
//

	}rf_message_data;
};

#endif
