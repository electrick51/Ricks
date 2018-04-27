/**
 * Filename: resources.c
 * Version: 1.0.0
 *
 * resources.c - holds common gm structures for screens to call get functions
 *
 */
#include "twttsdef.h"
#include "statusmanager.h"
#include "sensor.h"
#include "uimanager.h"
#include "dfformat.h"
#include "resources.h"
//#include "graphics_app_layer.h"
#include "xprintf.h"
#include "pnserial.h"

#define SIMULATION 0
#define INTERACTIVE_SIMULATION_KGS 0

#if INTERACTIVE_SIMULATION_KGS == 1
uint8_t interactiveServicesManager(uint8_t data[], uint8_t maxLength, uint8_t press);
#endif

//static const struct GRAPHICS_METHOD standardGM[] = 
//{
//    // close button
//    {
//        Control__Add,
//        .method_params.control_add_params = 
//        {Button,.Control.Button = {key_close, Visible | Enabled,DFI_BTN_CLOSE_STATIC ,{CLOSE_XY}}}
//    },
//    // emergency button
//    {
//        Control__Add,
//        .method_params.control_add_params = 
//        {Button,.Control.Button = {key_emergency,Visible | Enabled | UsePressedImage,DFI_BTN_EMERGENCY_STATIC,{7,6}}}
//    },
//    // header bevel
//    { 
//        DrawImage,
//        .method_params.image_params = {DFI_HEADER_BEVEL,{0,SCREEN_BEVEL_Y}} 
//    },
//    // screen title
//    { 
//        DrawImage,
//        .method_params.image_params = {0,{SCREEN_TITLE_X,SCREEN_TITLE_Y}} // {0,{18,50}}
//    },
//};

/**
 Function:      getGMstruct
 Parameters:    token = what kind of GM the calling function whants
                imageIndex = in the case of screen title, this function will return the graphics method with the appropriate imageIndex
 Returns:       graphics method struct
 Description:   in an attempt to save data on the const section, this will return common GM's to most screens
 Notes:         
*/
//struct GRAPHICS_METHOD getGMstruct(enum GRAPHICSMETHODTOKEN token, uint16_t imageIndex)
//{
//    struct GRAPHICS_METHOD gm = standardGM[token];
//    
//    switch (token)
//    {
//        case eScreenTitle:
//            gm.method_params.image_params.imageIndex = imageIndex;
//            break;
//    }
//    
//    return gm;
//}

/**
 Function:      getDStime
 Parameters:    none
 Returns:       draw_string_ex struct
 Description:   every screen who paints the time, can use this function instead of declaring their own structs in const space
 Notes:         
*/
//struct DRAW_STRING_EX getDStime(void)
//{
//    static const char null[] = "";
//    
//    const struct DRAW_STRING_EX drawString =         
//    { 
//        {{CLOCK_XY},Left,Medium,Normal,{255,255,255,255},null},
//        DFI_HEADER_SECONDARY,CLOCK_X,CLOCK_Y-13,56,13+3 //add 3 cuz "p" as in pm has pixels that hang below the line
//    };
//    
//    return drawString;
//}


//static uint8_t phone1ReportMode = 255;
/////////////////////////////////////////////////////// Access Codes
#if 0
static uint8_t accessCode[12][7] = {{"123456"},
                                    {"234567"},
                                    {"345678"},
                                    {"456789"},
                                    {"567890"},
                                    {"678901"},
                                    {"789012"},
                                    {"890123"},
                                    {"901234"},
                                    {"012345"},
                                    {"123456"},
                                    {"234567"},
                                   };
#endif
/*
/////////////////////////////////////////////////////// Phone Numbers
static uint8_t phoneNumber[4][11] = {{"1234567890"},
                                     {"2345678901"},
                                     {"3456789012"},
                                     {"4567890123"}
                                    };
                                                   
static uint8_t downloaderPhoneNumber[11] = {"5678901234"};

/////////////////////////////////////////////////////// Phone Options
static uint8_t manualPhoneTest = 1;
static uint8_t failToCommunicate = 1;
static uint8_t dtmfDial = 1;
static uint8_t bps300Enabled = 1;
static uint8_t ringHangRing = 1;
static uint8_t dialDelay = 30;
//static uint8_t callWaitingCode[10] = {1,2,3,4,5,6,7,8,9,0};
static uint8_t callWaitingCode[11] = {"1234567890"};

/////////////////////////////////////////////////////// Reporting Options
static uint8_t openingReports = 0;
static uint8_t closingReports = 0;
static uint8_t forceArmedReport = 0;
static uint8_t acPowerFailureReport = 0;
static uint8_t lowCPURbatteryReport = 1;
static uint8_t sensorAlarmRestoreReport = 0;
static uint8_t sensorTamper24hour = 0;
static uint8_t supervisoryTamperReport = 0;
static uint8_t noUsageReportReport = 0;
static uint8_t swingerShutdown = 1;
static uint8_t programmingReport = 0;
static uint8_t fireAlarmVerification = 0;

/////////////////////////////////////////////////////// Security Options
static uint8_t accountNumber[11] = {"1234567890"};
static uint8_t downloaderCode[6] = {"12345"};
static uint8_t phoneLock = 0;
static uint8_t autoArm = 1;
static uint8_t exitExtend = 1;
static uint8_t secureArming = 0;
static uint8_t noArmLowBatt = 0;
static uint8_t quickExit = 0;
static uint8_t downloaderEnable = 1;
static uint8_t supervisoryProtest = 0;

/////////////////////////////////////////////////////// Sensors
static uint8_t fakeSensor[3][16] = {{45,120,103,111,109,98,121,102,6,102,41,102,1,102,73,76},
                                    {45,120,103,111,109,98,121,102,6,102,41,102,1,102,73,75},
                                    {45,120,103,111,109,98,121,102,6,102,41,102,1,102,73,74}};

/////////////////////////////////////////////////////// Siren Options
static uint8_t panelPiezoBeeps = 1;
static uint8_t panelVoice = 1;
static uint8_t panelPiezoAlarm = 1;
static uint8_t troubleBeeps = 1;
static uint8_t voiceChime = 0;
static uint8_t statusBeepsVolume = 7;
static uint8_t hardwiredSirenSupervision = 0;
static uint8_t speakerVolume = 8;
static uint8_t silentPolicePanic = 0;
static uint8_t panelTamperAlarm = 0;
static uint8_t alarmReportVerify = 0;

/////////////////////////////////////////////////////// System Options
static uint8_t rfJamDetect = 0;
static uint8_t demoMode = 0;
static uint8_t hw1function = 1;
static uint8_t clockFormat24hour = 0;

/////////////////////////////////////////////////////// Timers

static struct MYTIME latchkeyTime = {1,{0,15,17,0,0,0,0,0,0}};
static struct MYTIME supervisoryTime = {0,{0,0,0,0,0,0,0,0,0}};
static struct MYTIME failToOpenTime = {0,{0,0,0,0,0,0,0,0,0}};
static struct MYTIME failToCloseTime = {0,{0,0,0,0,0,0,0,0,0}};


/////////////////////////////////////////////////////// Touchpad Options
static uint8_t keyfobNoDelay = 0;
static uint8_t panicAlarms = 1;
static uint8_t remoteTParm = 0;*/

//*****************************************************

/**
 Function:      getPanelData
 Parameters:    token = depicts which peice of the calling function wants to get
                panelData = pointer to where to put the return data
 Returns:       enumeration depicting what the status of that peice of data is
 Description:   when one wants to get data (usually programming data) from the panel...we use this
 Notes:         
*/
enum RESOURCESTATUS getPanelData(uint16_t token, struct PANELDATA *panelData)
{    
    enum RESOURCESTATUS resourceStatus = eUIItemOk;
	panelData->token = token;

#if INTERACTIVE_SIMULATION_KGS  == 1
    if (token == tInteractiveScreenScript)
    {
        panelData->inputDataLen = interactiveServicesManager(panelData->data.array, sizeof (panelData->data.array) + sizeof (panelData->dataInfo) -1, 0xFF);
        if (panelData->inputDataLen == 0)
        {
            resourceStatus = eUIItemNotAvailable;
        }
        return resourceStatus;
    }
#endif

#if 1
    resourceStatus = readPanelData(panelData);
#else
    switch (panelData->token)
    {
        //access codes
        case tDealerCode:
        case tInstallerCode:
        case tMasterCode:
        case tUserCode1:
        case tUserCode2:
        case tUserCode3:
        case tUserCode4:
        case tUserCode5:
        case tUserCode6:
        case tUserCode7:
        case tUserCode8:
        case tDuressCode:
#if SIMULATION == 0
			resourceStatus = readPanelData(panelData);
            break;
#else
			memcpy(&panelData->data.array[0], &accessCode[token-tDealerCode][0], 6);
			panelData->dataType = eARRAY;
            strcpy((char*)&panelData->dataInfo[0], "\x1bv0-9;*;#;\x1bh6;\x1bl6;\x1bx123456;\x1bm\x02");
            break;
#endif
        case tCodeSize:
        //audio verification
        case tAudioMode:
        case tFireShutdown:
        case tPanicTalk:
        case tVOXreceiverGain:
        case tVOXmicrophoneGain:
        case tVOXgainRange:
        case tManualMicrophoneGain:
		case tFireEnabledAvm:
        //communication modes
        case tPhone1Reports:

        case tPhone2Reports:
        case tPhone3Reports:
        case tPhone4Reports:
			resourceStatus = readPanelData(panelData);
            break;
        
        //light control
        
        //phone numbers
        case tPhoneNumber1:
        case tPhoneNumber2:
        case tPhoneNumber3:
        case tPhoneNumber4:
            break;
        case tDownloaderNumber:
			resourceStatus = readPanelData(panelData);
            break;
        
        //phone options
        case tManualPhoneTest:
			resourceStatus = readPanelData(panelData);
            break;
        case tFailToCommunicate:
			resourceStatus = readPanelData(panelData);
            break;
        case tDTMFdialing:
			resourceStatus = readPanelData(panelData);
            break;
        case t300bpsEnabled:
			resourceStatus = readPanelData(panelData);
            break;
        case tRingHangRing:
			resourceStatus = readPanelData(panelData);
            break;
        case tDialerDelay:
			resourceStatus = readPanelData(panelData);
            break;
        case tCallWaitingCode:
			resourceStatus = readPanelData(panelData);
            break;
        
        //reporting options
        case tOpeningReport:
        case tClosingReport:
        case tForceArmedReport:
        case tACpowerFailureReport:
        case tPanelLowBatteryReport:
        case tSensorAlarmRestoralReport:
        case t24hourSensorTamper:
        case tSupervisoryTamperReport:
        case tNoUsageReport:
        case tSwingerShutdown:
        case tProgrammingReport:
        case tFireAlarmVerification:
			resourceStatus = readPanelData(panelData);
            break;
        
        //reporting
        
        //programming

        //security
        case tAccountNumber:
        case tDownloaderCode:
        case tPhoneLock:
        case tAutoArm:
        case tExitExtension:
        case tSecureArming:
        case tNoArmLowBatt:
        case tQuickExit:
        case tDownloaderEnable:
        case tSupervisoryProtest:
			resourceStatus = readPanelData(panelData);
            break;
        
        //sensors
        case tSensor01:
        case tSensor02:
        case tSensor03:
        case tSensor04:
        case tSensor05:
        case tSensor06:
        case tSensor07:
        case tSensor08:
        case tSensor09:
        case tSensor10:
        case tSensor11:
        case tSensor12:
        case tSensor13:
        case tSensor14:
        case tSensor15:
        case tSensor16:
        case tSensor17:
        case tSensor18:
        case tSensor19:
        case tSensor20:
        case tSensor21:
        case tSensor22:
        case tSensor23:
        case tSensor24:
        case tSensor25:
        case tSensor26:
        case tSensor27:
        case tSensor28:
        case tSensor29:
        case tSensor30:
        case tSensor31:
        case tSensor32:
        case tSensor33:
        case tSensor34:
        case tSensor35:
        case tSensor36:
        case tSensor37:
        case tSensor38:
        case tSensor39:
        case tSensor40:
            #if SIMULATION == 0
                resourceStatus = readPanelData(panelData);
                break;
            #else
                memcpy(&panelData->data.array[0], &fakeSensor[token - tSensor01][0], 16);
                panelData->dataType = eSENSORTOKEN;
                strcpy((char*)&panelData->dataInfo[0], "\x1bv0-198;255;");
                strcat((char*)&panelData->dataInfo[0], "\x1bx255;");
                break; 
            #endif
        //sensors L2

        //siren options
        case tPanelPiezoBeeps:
        case tPanelVoice:
        case tPanelPiezoAlarms:
        case tTroubleBeeps:
        case tVoiceChime:
        case tStatusBeepVolume:
        case tHardwiredSirenSupervision:
        case tSpeakerVolume:
        case tPanelSilentPolicePanic:
        case tPanelTamperAlarm:
        case tAlarmReportVerification:
			resourceStatus = readPanelData(panelData);
            break;

        //system options
        case tRFjamDetect:
        case tDemoMode:
        case tHW1function:
        case tClockFormat24hour:
			resourceStatus = readPanelData(panelData);
            break;
        
        //system tests

        //timers
        case tLatchkeyTime:
            #if SIMULATION == 0
                resourceStatus = readPanelData(panelData);
                break;
            #else
                panelData->data.time = latchkeyTime;
                panelData->dataType = eTIME;
                break;
            #endif
        case tEntryDelay:
        case tExitDelay:
        case tNoActivityTimeout:
        case tAutoPhoneTest:
			readPanelData(panelData);
			break;
        case tSupervisoryTime:
            #if SIMULATION == 0
                resourceStatus = readPanelData(panelData);
                break;
            #else
                panelData->data.time = supervisoryTime;
                panelData->dataType = eTIME;
                break;
            #endif
        case tAlarmCancelWindow:
        case tRFtimeout:
			readPanelData(panelData);
            break;
        case tFailToOpenTime:
            #if SIMULATION == 0
                resourceStatus = readPanelData(panelData);
                break;
            #else
                panelData->data.time = failToOpenTime;
                panelData->dataType = eTIME;
                break;
            #endif
        case tFailToCloseTime:
            #if SIMULATION == 0
                resourceStatus = readPanelData(panelData);
                break;
            #else
                panelData->data.time = failToCloseTime;
                panelData->dataType = eTIME;
                break;
            #endif
        case tSirenTimeout:
        case tArmingLEDshutdown:
        case tUnvacatedPremises:
        case tSmokeSensorSupervision:
			readPanelData(panelData);
            break;

        //touchpad options
        case tKeyfobNoDelay:
        case tPanelPanicAlarms:
        case tRemoteTouchpadArming:
			resourceStatus = readPanelData(panelData);
            break;
        
        //access code check
        case tAccessCodeCheck:
            //resourceStatus = readPanelData(panelData);
            resourceStatus = eUIItemOk;
            
            break;
            
        default:
            break;
    }
#endif
    return resourceStatus;
}

/**
 Function:      setPanelData
 Parameters:    token = depicts which peice of the calling function wants to set
                panelData = pointer to where the data to set is
 Returns:       ?
 Description:   when one wants to set data (usually programming data) in the panel...we use this
 Notes:         
*/
uint8_t setPanelData(struct PANELDATA *panelData, uint16_t token)
{   
    panelData->token = token;  

#if INTERACTIVE_SIMULATION_KGS  == 1       
    switch (token)
    {    
        case tInteractiveScreenKeyPress:        //send keypress to the panel
            interactiveServicesManager(NULL, 0, panelData->data.byte);
            return 0;
        case tInteractiveServicesEntered:
            interactiveServicesManager(NULL, 0, 0xff);
            return 0;
        case tInteractiveServicesExit:
            return 0;
    }
#endif
  
    writePanelData(panelData);/*

	//panelData->inputDataLen = 0;

	switch (token)
    {
        //access codes
        case tDealerCode:
        case tInstallerCode:
        case tMasterCode:
        case tUserCode1:
        case tUserCode2:
        case tUserCode3:
        case tUserCode4:
        case tUserCode5:
        case tUserCode6:
        case tUserCode7:
        case tUserCode8:
        case tDuressCode:
#if SIMULATION == 0
			panelData->inputDataLen = 4; //KGS TBD remove
			writePanelData(panelData);
            break;
#else
            //memcpy(&accessCode[token-tDealerCode][0], &panelData->data.array[0], accessCodeSize);
            strcpy((char*)&accessCode[token-tDealerCode][0], (char*)&panelData->data.array[0]);
            break;
#endif
        case tCodeSize:
        //audio verification
        case tAudioMode:
        case tFireShutdown:
        case tPanicTalk:
        case tVOXreceiverGain:
        case tVOXmicrophoneGain:
        case tVOXgainRange:
        case tManualMicrophoneGain:
		case tFireEnabledAvm:
        //communication modes
        case tPhone1Reports:
        case tPhone2Reports:
        case tPhone3Reports:
        case tPhone4Reports:
			writePanelData(panelData);
        
        //light control
        
        //phone numbers
        case tPhoneNumber1:
        case tPhoneNumber2:
        case tPhoneNumber3:
        case tPhoneNumber4:
            memcpy(&phoneNumber[token-tPhoneNumber1][0], &panelData->data.array[0], 10);
            break;
        case tDownloaderNumber:
            memcpy(&downloaderPhoneNumber[0], &panelData->data.array[0], 10);
            break;
        
        //phone options
        case tManualPhoneTest:
			writePanelData(panelData);
            break;
        case tFailToCommunicate:
			writePanelData(panelData);
            break;
        case tDTMFdialing:
			writePanelData(panelData);
            break;
        case t300bpsEnabled:
			writePanelData(panelData);
            break;
        case tRingHangRing:
			writePanelData(panelData);
            break;
        case tDialerDelay:
			writePanelData(panelData);
            break;
        case tCallWaitingCode:
            memcpy(&callWaitingCode[0], &panelData->data.array[0], 10);
            break;
        
        //reporting options
        case tOpeningReport:
            openingReports = panelData->data.byte;
            break;
        case tClosingReport:
            closingReports = panelData->data.byte;
            break;
        case tForceArmedReport:
            forceArmedReport = panelData->data.byte;
            break;
        case tACpowerFailureReport:
            acPowerFailureReport = panelData->data.byte;
            break;
        case tPanelLowBatteryReport:
            lowCPURbatteryReport = panelData->data.byte;
            break;
        case tSensorAlarmRestoralReport:
            sensorAlarmRestoreReport = panelData->data.byte;
            break;
        case t24hourSensorTamper:
            sensorTamper24hour = panelData->data.byte;
            break;
        case tSupervisoryTamperReport:
            supervisoryTamperReport = panelData->data.byte;
            break;
        case tNoUsageReport:
            noUsageReportReport = panelData->data.byte;
            break;
        case tSwingerShutdown:
            swingerShutdown = panelData->data.byte;
            break;
        case tProgrammingReport:
            programmingReport = panelData->data.byte;
            break;
        case tFireAlarmVerification:
            fireAlarmVerification = panelData->data.byte;
            break;
        
        //reporting
        
        //programming
        
        //security
        case tAccountNumber:
            memcpy(&accountNumber[0], &panelData->data.array[0], 10);
            break;
        case tDownloaderCode:
            memcpy(&downloaderCode[0], &panelData->data.array[0], 10);
            break;
        case tPhoneLock:
            phoneLock = panelData->data.byte;
            break;
        case tAutoArm:
            autoArm = panelData->data.byte;
            break;
        case tExitExtension:
            exitExtend = panelData->data.byte;
            break;
        case tSecureArming:
            secureArming = panelData->data.byte;
            break;
        case tNoArmLowBatt:
            noArmLowBatt = panelData->data.byte;
            break;
        case tQuickExit:
            quickExit = panelData->data.byte;
            break;
        case tDownloaderEnable:
            downloaderEnable = panelData->data.byte;
            break;
        case tSupervisoryProtest:
            supervisoryProtest = panelData->data.byte;
            break;
        
        //sensors
        case tSensor01:
        case tSensor02:
        case tSensor03:
        case tSensor04:
        case tSensor05:
        case tSensor06:
        case tSensor07:
        case tSensor08:
        case tSensor09:
        case tSensor10:
        case tSensor11:
        case tSensor12:
        case tSensor13:
        case tSensor14:
        case tSensor15:
        case tSensor16:
        case tSensor17:
        case tSensor18:
        case tSensor19:
        case tSensor20:
        case tSensor21:
        case tSensor22:
        case tSensor23:
        case tSensor24:
        case tSensor25:
        case tSensor26:
        case tSensor27:
        case tSensor28:
        case tSensor29:
        case tSensor30:
        case tSensor31:
        case tSensor32:
        case tSensor33:
        case tSensor34:
        case tSensor35:
        case tSensor36:
        case tSensor37:
        case tSensor38:
        case tSensor39:
        case tSensor40:
            memcpy(&fakeSensor[token - tSensor01][0], &panelData->data.array[0], 16);
            break;
        //sensors L2
        
        //siren options
        case tPanelPiezoBeeps:
            panelPiezoBeeps = panelData->data.byte;
            break;
        case tPanelVoice:
            panelVoice = panelData->data.byte;
            break;
        case tPanelPiezoAlarms:
            panelPiezoAlarm = panelData->data.byte;
            break;
        case tTroubleBeeps:
            troubleBeeps = panelData->data.byte;
            break;
        case tVoiceChime:
            voiceChime = panelData->data.byte;
            break;
        case tStatusBeepVolume:
            statusBeepsVolume = panelData->data.byte;
            break;
        case tHardwiredSirenSupervision:
            hardwiredSirenSupervision = panelData->data.byte;
            break;
        case tSpeakerVolume:
            speakerVolume = panelData->data.byte;
            break;
        case tPanelSilentPolicePanic:
            silentPolicePanic = panelData->data.byte;
            break;
        case tPanelTamperAlarm:
            panelTamperAlarm = panelData->data.byte;
            break;
        case tAlarmReportVerification:
            alarmReportVerify = panelData->data.byte;
            break;
        
        //system options
        case tRFjamDetect:
            rfJamDetect = panelData->data.byte;
            break;
        case tDemoMode:
            demoMode = panelData->data.byte;
            break;
        case tHW1function:
            hw1function = panelData->data.byte;
            break;
        case tClockFormat24hour:
            clockFormat24hour = panelData->data.byte;
            break;
        
        //system tests
        
        //timers
        case tLatchkeyTime:
            latchkeyTime = panelData->data.time;
            break;
        case tEntryDelay:
        case tExitDelay:
        case tNoActivityTimeout:
		case tAutoPhoneTest:
			writePanelData(panelData);
            break;
        case tSupervisoryTime:
            supervisoryTime = panelData->data.time;
            break;
        case tAlarmCancelWindow:
        case tRFtimeout:
			writePanelData(panelData);
            break;
        case tFailToOpenTime:
            failToOpenTime = panelData->data.time;
            break;
        case tFailToCloseTime:
            failToCloseTime = panelData->data.time;
            break;
        case tSirenTimeout:
        case tArmingLEDshutdown:
        case tUnvacatedPremises:
        case tSmokeSensorSupervision:
			writePanelData(panelData);
            break;
        
        //touchpad options
        case tKeyfobNoDelay:
            keyfobNoDelay = panelData->data.byte;
            break;
        case tPanelPanicAlarms:
            panicAlarms = panelData->data.byte;
            break;
        case tRemoteTouchpadArming:
            remoteTParm = panelData->data.byte;
            break;
            
        default:
            break;
    }
    */
    return 1;
}

/**********************************************************************************************
 **********************************************************************************************
  KGS CODE FROM HERE ON DOWN IS FOR SIMULATION ONLY.... NOT TO BE ENABLED FOR RELEASED PRODUCT
 **********************************************************************************************
 **********************************************************************************************/
#if INTERACTIVE_SIMULATION_KGS == 1

uint8_t interactiveServicesManager(uint8_t data[], uint8_t maxLength, uint8_t press)
{
	static uint8_t whichStringToPrint = 0;
/*
    uint8_t *myString[] = {"\x1b\x10"
                         "\x1b\x09\x00"                      //justification - left
                         "\x1b\x02\x12\x00\x18\x00"       //set the text start position
                         "\x1b\x01\x01\x01"               //set font:medium and bold
                         "\x1b\x00\xff\xff\xff"         //set the color:white
                         "Interactive Services"
                         ,
                         "\x1b\x10"
                         "\x1b\x04\x05\x00\x2E\x00\x71\x01"         // input window
                         "\x1b\x04\xC8\x00\x91\x00\x71\x01"         // value window
                         "\x1b\x0A\xEC\x00\x69\x00\xD5\x00\x01\xC1" // up
                         "\x1b\x0A\xEC\x00\xB0\x00\xD7\x00\x01\xC2" // down
                         "\x1b\x0A\xE3\x00\x0B\x00\xDB\x00\x01\xC0" // close
                         ,
                         "\x1b\x10"
                         "\x1b\x0A\x7D\x00\x2E\x00\x74\x01\x00\xC3" // ok
                         "\x1b\x0A\x7D\x00\x4B\x00\x70\x01\x00\xC4" // backspace
                         "\x1b\x0A\x05\x00\x4B\x00\x3E\x00\x01\x01" // 1
                         "\x1b\x0A\x2D\x00\x4B\x00\x40\x00\x01\x02" // 2
                         "\x1b\x0A\x55\x00\x4B\x00\x42\x00\x01\x03" // 3
                         ,
                         "\x1b\x10"
                         "\x1b\x0A\x05\x00\x73\x00\x44\x00\x01\x04" // 4
                         "\x1b\x0A\x2D\x00\x73\x00\x46\x00\x01\x05" // 5
                         "\x1b\x0A\x55\x00\x73\x00\x48\x00\x01\x06" // 6
                         "\x1b\x0A\x05\x00\x9B\x00\x4A\x00\x01\x07" // 7
                         "\x1b\x0A\x2D\x00\x9B\x00\x4C\x00\x01\x08" // 8
                         ,
                         "\x1b\x10"
                         "\x1b\x0A\x55\x00\x9B\x00\x4E\x00\x01\x09" // 9
                         "\x1b\x0A\x05\x00\xC3\x00\x52\x00\x01\x0B" // *
                         "\x1b\x0A\x2D\x00\xC3\x00\x3C\x00\x01\x00" // 0
                         "\x1b\x0A\x55\x00\xC3\x00\x50\x00\x01\x0A" // #
                         ,
                         ""}
                         ;
*/
const uint8_t *myString[] = {"\x1b\x10"
                       "\x1b\x0C"
                       "\x1b\x09\x00"                      //justification - left
                       "\x1b\x01\x01\x01"               //set font:medium and bold
                       "\x1b\x00\xff\xff\xff"         //set the color:white
                       "\x1b\x02\x12\x00\x18\x00"       //set the text start position
                       "Interactive Services"
                       "\x1b\x0A\xE3\x00\x0B\x00\xDB\x00\x01\xC0" // close
                       ,
                       "\x1b\x10"
                       "\x1b\x02\x2A\x00\x43\x00"       //set the text start position
                       "Checkbox 0"
                       "\x1b\x0A\x0D\x00\x31\x00\x8e\x01\x00\x01" //checkbox 0 - 0x18e =  DFI_CHECKBOX_UNCHECKED, control id = x01
                       ,
                       "\x1b\x10"
                       "\x1b\x02\x2A\x00\x5D\x00"       //set the text start position
                       "Checkbox 1"
                       "\x1b\x0A\x0D\x00\x4B\x00\x8e\x01\x00\x02" //checkbox 1 - 0x18e =  DFI_CHECKBOX_UNCHECKED, control id = 0x02
                       ,
                       "\x1b\x10"
                       "\x1b\x02\x2A\x00\x77\x00"       //set the text start position
                       "Checkbox 2"
                       "\x1b\x0A\x0D\x00\x65\x00\x8e\x01\x00\x03" //checkbox 2 - 0x18e =  DFI_CHECKBOX_UNCHECKED, control id = 0x02
                       ,
                       "\x1b\x10"
                       "\x1b\x02\x2A\x00\x91\x00"       //set the text start position
                       "Checkbox 3"
                       "\x1b\x0A\x0D\x00\x7F\x00\x8e\x01\x00\x04" //checkbox 3 - 0x18e =  DFI_CHECKBOX_UNCHECKED, control id = 0x02
                       ,
                       "\x1b\x10"
                       "\x1b\x02\x2A\x00\xAB\x00"       //set the text start position
                       "Checkbox 4"
                       "\x1b\x0A\x0D\x00\x99\x00\x8e\x01\x00\x05" //checkbox 4 - 0x18e =  DFI_CHECKBOX_UNCHECKED, control id = 0x02
                       ,
                       "\x1b\x10"
                       "\x1b\x02\x2A\x00\xC5\x00"       //set the text start position
                       "Checkbox 5"
                       "\x1b\x0A\x0D\x00\xB3\x00\x8e\x01\x00\x06" //checkbox 5 - 0x18e =  DFI_CHECKBOX_UNCHECKED, control id = 0x02
                       ,
                       "\x1b\x10"
                       "\x1b\x02\x2A\x00\xDF\x00"       //set the text start position
                       "Checkbox 6"
                       "\x1b\x0A\x0D\x00\xCD\x00\x8e\x01\x00\x07" //checkbox 6 - 0x18e =  DFI_CHECKBOX_UNCHECKED
                       ,
                       "\x1b\x10"
                       "\x1b\x0A\x00\x01\x6B\x00\x31\x01\x00\x08" //cross DFI_CROSS = 305 = 0x131
                       ,
                       ""};
                         
uint8_t nextScreen[] = 
                             "\x1b\x10"
                             "\x1b\x0C"
                             "\x1b\x02\x12\x00\x18\x00"       //set the text start position
                             "New Screen"
                             "\x1b\x0A\xE3\x00\x0B\x00\xDB\x00\x01\xC0"; // close
uint8_t replaceCheckBox[] =
                             "\x1b\x10"
                             "\x1b\x0B\x01"                                 //remove control command
                             "\x1b\x0A\x0D\x00\x00\x00\x8e\x01\x00\x01";    //add control command
                             
    static uint8_t lastKey = 0xFF;
    static uint8_t checkBoxState[7] = {0,0,0,0,0,0,0};
    static uint8_t depth = 0;
    uint8_t printIt = 0;
	uint8_t dataLen = 0;
    
    if (data == NULL)
    {
        lastKey = press;
        if (lastKey == 0xFF)
        {
            whichStringToPrint = 0;
        }
    }
    else
    {
        if (lastKey == 0xC0)            //close key
        {
            lastKey = 0xFF;
            checkBoxState[0] = 0;
            checkBoxState[1] = 0;
            checkBoxState[2] = 0;
            checkBoxState[3] = 0;
            checkBoxState[4] = 0;
            checkBoxState[5] = 0;
            checkBoxState[6] = 0;
            whichStringToPrint = 0;
            if (depth == 0)
            {
                strcpy((char*)&data[0], "\x1b\x11");				//leave interactive services mode
            	dataLen = 2;
			}
            else
            {
                depth--;
                printIt = 1;
            }
        }
        else if (lastKey <= 7)          //check box keys
        {
            replaceCheckBox[4] = lastKey;
            replaceCheckBox[14] = lastKey;
            replaceCheckBox[9] = 0x31+((lastKey-1)*0x1A);           //0x31 is the y position of checkbox 0, 0x1a is the spacing for each check box
            checkBoxState[lastKey-1] = !checkBoxState[lastKey-1];   //use -1 because checkbox 0's control id starts at 1
            replaceCheckBox[11] = 0x8e + checkBoxState[lastKey-1];   //0x8e is lower byte of DFI_CHECKBOX_UNCHECKED
            dataLen = sizeof(replaceCheckBox);            
			memcpy(&data[0], &replaceCheckBox[0], dataLen);
			lastKey = 0xFF;
        }
        else if (lastKey == 0x08)       //cross key
        {
            dataLen = sizeof(nextScreen);           
			memcpy(&data[0], &nextScreen[0], dataLen);
            lastKey = 0xFF;
            depth++;
        }
        else if (lastKey == 0xFF)
        {
            printIt = 1;
        }       
        if (printIt == 1)
        {
            printIt = 0;
			dataLen = 0;
			if (*myString[whichStringToPrint])	//0 in first byte of script => end of myString[]
			{
				dataLen = myString[whichStringToPrint+1] - myString[whichStringToPrint];
                dataLen -= 1;   //remove the null character from the length
			}            	
			memcpy(&data[0], myString[whichStringToPrint], dataLen);
            if (dataLen)
            {
                whichStringToPrint++;
            }
        }
    }
    return dataLen;
}
#endif
