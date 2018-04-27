#ifndef RESOURCES_H
#define RESOURCES_H

#include <time.h>
#include "dlpktdef.h"
//#include "touchscreen.h"

struct DRAW_STRING_EX getDStime(void);
/*
struct MYTIME
{
    uint8_t timeIsSet;
    struct tm tm;
};

#define MAXPANELARRAYSIZE 50

union INPUTDATATYPES
{
    uint8_t byte;
    uint8_t token;
    uint16_t uint;
    struct MYTIME time;
    uint8_t array[MAXPANELARRAYSIZE];
};

enum DATATYPE
{
    eNoType=0,
    eUINT8_T,
    eUINT16_T,
    eARRAY,
    eSENSORTOKEN,
    eCHECKBOX,
    eTIME,
    eSPECIAL,
    eTESTING,
//  eINT8_T,
//  eINT16_T,
};*/

#define SCREEN_TITLE_X 18
#define SCREEN_TITLE_Y 50 // 50
#define SCREEN_TITLE_XY SCREEN_TITLE_X,SCREEN_TITLE_Y

struct DATABOUNDRY
{
    uint16_t lowerBound;
    uint16_t upperBound;
};

struct CHECKINFO
{
    uint8_t (*dataCheck)(uint16_t, uint16_t, uint16_t, uint16_t);
    struct DATABOUNDRY boundry;
    uint8_t arrayMaxLength;
    uint8_t deletedVal; //
};

enum TESTTYPES
{
   eBoundry = 0x01,
   eLength = 0x02,
   /*a = 0x04,
   a = 0x08,
   a = 0x20,
   a = 0x10,
   a = 0x40,
   a = 0x80,*/
};
/*
#define MAXSCRIPTSTRINGSIZE 50

struct PANELDATA
{
    uint8_t dataInfo[MAXSCRIPTSTRINGSIZE];
    enum DATATYPE dataType;
    union INPUTDATATYPES data;
};

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
    tUserCode7,
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
    tManualMicrophoneGain,
    
    //communication modes
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
    tDownloaderNumber,
    
    //phone options
    tManualPhoneTest,
    tFailToCommunicate,
    tDTMFdialing,
    t300bpsEnabled,
    tRingHangRing,
    tDialerDelay,
    tCallWaitingCode,
    
    //reporting options
    tOpeningReport,   
    tClosingReport,
    tForceArmedReport,
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
    tAccountNumber,
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
    
    //sensors L2
    
    //siren options
    tPanelPiezoBeeps,
    tPanelVoice,
    tPanelPiezoAlarms,
    tTroubleBeeps,
    tVoiceChime,
    tStatusBeepVolume,
    tHardwiredSirenSupervision,
    tSpeakerVolume,
    tPanelSilentPolicePanic,
    tPanelTamperAlarm,
    tAlarmReportVerification,
    
    //system options
    tRFjamDetect,
    tDemoMode,
    tHW1function,
    tClockFormat24hour,
    
    //system tests
    
    //timers
    tLatchkeyTime,   
    tEntryDelay,
    tExitDelay,
    tNoActivityTimeout,
    tAutoPhoneTest,
    tSupervisoryTime,
    tAlarmCancelWindow,
    tRFtimeout,
    tFailToOpenTime,
    tFailToCloseTime,
    tSirenTimeout,
    tArmingLEDshutdown,
    tUnvacatedPremises,
    tSmokeSensorSupervision,
    
    //touchpad options
    tKeyfobNoDelay,
    tPanelPanicAlarms,
    tRemoteTouchpadArming,
};*/

enum GRAPHICSMETHODTOKEN
{
    //eTime = 0,
    eClose = 0,
    eEmergency,
    eHeaderBevel,
    eScreenTitle,
};

enum RESOURCESTATUS
{
	eUIItemOk = 0,
	eUIItemNotAvailable = 1,
	eUIItemWait = 2,
	eUIItemRangeError = 3,
    eUIItemError = 4,
    eUIItemNotSet = 5,
    eUIItemScript = 6	
};

struct GRAPHICS_METHOD getGMstruct(enum GRAPHICSMETHODTOKEN token, uint16_t imageIndex);

enum RESOURCESTATUS getPanelData(uint16_t token, struct PANELDATA *panelData);
uint8_t setPanelData(struct PANELDATA *panelData, uint16_t token);


#endif /* RESOURCES_H */
