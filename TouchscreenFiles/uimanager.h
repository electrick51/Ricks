/**
 * Filename: uimanager.h
 *  Vrsion: 1.0.0
 *
 *  Header for uimanager.h"
 */
#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#ifndef QUEUE_H
#include "queue.h"
#endif
#include "Timers.h"

#define ADDITIONAL_PIXELS_BELOW_PROGRAMMING_LISTBOX_ROWS 5
#define CROPPED_SPACING_TO_NEXT_ITEM 5
//#define X_OFFSET(x)     x * 1.5
//#define Y_OFFSET(x)     x * 1.1

//
// Graphical coordinates for system wide controls
//
#define EMERGENCY_XY 7,6
#define CLOSE_X 377
#define CLOSE_Y 7
#define CLOSE_XY CLOSE_X, CLOSE_Y // 227, 11
//#define SCROLL_UP_XY 267,102
//#define SCROLL_DOWN_XY 267,181
#define CANCEL_XY CLOSE_XY

#define OK_XY CLOSE_XY

#define SCREEN_BEVEL_Y 75
#define HEADER_BEVEL_XY 0,SCREEN_BEVEL_Y
#define CLOCK_X 215//135
#define CLOCK_Y 20 
#define CLOCK_XY CLOCK_X,CLOCK_Y

#define NO_DISPLAY_ELEMENTS_Y 120
#define NO_DISPLAY_ELEMENTS_XY SCREEN_TITLE_X,NO_DISPLAY_ELEMENTS_Y

#define SCROLL_X 410
#define SCROLL_UP_Y 113
#define SCROLL_DWN_Y (SCROLL_UP_Y+79)

#define SCROLL_UP_XY SCROLL_X,SCROLL_UP_Y
#define SCROLL_DOWN_XY SCROLL_X,SCROLL_DWN_Y

enum SCREEN_FOCUS   { 
                        sc_invalid_screen=0,    //screens alphabetized     
                        
                        sc_ad_screen,
                        sc_armcountdwn_screen,
                        //sc_armcountdwn_w_bypass_screen,
                        sc_blank_screen,
                        sc_calibration_screen,
                        sc_dealer_contact_screen,
                        sc_dealer_promo_screen,
                        sc_doorlock_screen,
                        sc_emergency_screen,
                        sc_event_screen,
                        sc_faq_screen,
                        sc_help_screen,
                        //sc_help_screen_b,
                        sc_information_screen,
                        sc_keypad_screen,
                        //sc_keypad_screen_entry_delay,
                        //sc_keypad_screen_doorlock,
                        sc_light_screen,
                        sc_main_screen,
                        sc_pre_arm_screen,
                        sc_protest_screen,
                        sc_sensor_screen,
                        sc_tstatus_screen,
                        sc_text_screen,
                        sc_thermostat_screen,
                        sc_weather_screen,
                        sc_modification_screen,
                        
                        sc_programming_screen,
                        sc_programming_access_codes_screen,
                        sc_programming_security_screen,
                        sc_programming_phone_numbers_screen,
                        sc_programming_phone_options_screen,
                        sc_programming_sensors_screen,
                      	sc_programming_sensors_screen_L2,
                      	sc_programming_sensors_screen_L3,
                        sc_programming_sensors_learn_screen,
                        sc_programming_sensors_learn_screen_L2,
                        sc_programming_reporting_screen,
                    	sc_programming_reporting_options_screen,
                  		sc_programming_communication_modes_screen,
                        sc_programming_timers_screen,
                        sc_programming_touchpad_options_screen,
                        sc_programming_system_options_screen,
                        sc_programming_siren_options_screen,
                        sc_programming_audio_verification_screen,
                        sc_programming_light_control_screen,
                        sc_programming_system_tests_screen,
                        sc_test_result_screen,
                        sc_sensor_test_screen,                        
                        sc_interactive_services_screen,
                    	sc_programming_learn_sensors_screen,
                        sc_programming_modify_sensor_name_screen,
                        sc_programming_set_date_time_screen,
                        sc_directBypass_screen,
                        sc_photograph_screen,
                        sc_photograph_screen_L2,
                        sc_sensor_photograph_screen,
                        sc_same_screen = 0xfe,
                        sc_back_one_screen = 0xff,
                    };
                    

#define SCREENSTACKUPBTYES 10
#define SCREENSTACKUPPTRS SCREENSTACKUPBTYES/2
/*
union SCREEN_STACKUP_DATA
{
    void *ptr[SCREENSTACKUPPTRS];
    uint8_t array[SCREENSTACKUPBTYES];
};*/

enum SCREEN_STACKUP_DATA_TYPE
{
    eProgrammingScreens = 0,
    eGeneral,
    eProgrammingCodeEntry,
    eRuntimeCodeEntry
};

struct SCREEN_STACKUP_DATA
{
    enum SCREEN_STACKUP_DATA_TYPE type;
    
    union
    {
        uint8_t screenAfterKeypad;
        
        struct
        {
            uint16_t dataToken;
            uint16_t stringToken;
            uint8_t mode;
            uint8_t sensorNbr;              //depending on the program screen sensorNbr
            uint8_t sensorTextI;            //depending on the program screen sensorTextI may not be used               
        }programmingData;
        
        struct
        {
            void *ptr[SCREENSTACKUPPTRS];
            uint8_t array[SCREENSTACKUPBTYES];
        }generalData;
    }data;
};

struct SCREEN_STACKUP
{
    enum SCREEN_FOCUS screenToDisplay;
    uint16_t rowNumber;
    struct SCREEN_STACKUP_DATA screenData;
};
                    
#define SCREEN_TMOUT_SEC 60//USE 10 seconds for testing KGS 300               //number of seconds screen will remain in focus before returning to main (or arm level)
                        
void ui_manager_thread_init(void);
struct Message_Data *ui_message_get(void);
void ui_message_put(struct Message_Data *new_msg);
struct Message_Data *ui_message_get(void);
void uimanager_message_init(void);
///uint8_t relinquish_focus(void);
uint8_t relinquish_focus(uint16_t timeoutStart, uint16_t timeoutSeconds);
struct GRAPHICS_METHOD getGMremoveAll();
void ClearOneStackupEntry(struct SCREEN_STACKUP *screenEntry);

//phrase tokens as defined by SC10... see ui.h for sc10
//these token must match sc10 tokens.. TBD remove???

#define T_None                    0
#define T_None1                   1       	    
#define T_LightScheds             2       
#define T_SensorLights            3
#define T_Chime                   4
#define T_SpecialChm              5
#define T_SystemTests             6
#define T_SystemProg              7
#define T_ViewEvents              8
#define T_ByPass                  9
#define T_SensorName              10
#define T_BeepLow                 11
#define T_SensorMapX10Nbr         12
#define T_Security 	              13
#define T_AccessCds               14
#define T_Sensors                 15
#define T_EnterCode               16
#define T_EtherNet                17
#define T_ACOKPHRASE              18
#define T_AlarmText               19
#define T_TripNewSensor           20
#define T_SystemOk 	              21
#define T_DisarmFirst             22
#define T_SetClock                23
#define T_Time                    24
#define T_PhoneOptions            25   //this is used in the phone number to indicate a pause
#define T_ManualPhTest            26   //back to previous menu
#define T_FailToCom               27
#define T_DTMF_DIAL               28
#define T_300BPS                  29
#define T_ModemSens               30
#define T_RingHangRing            31
#define T_DialDelay               32
#define T_CallWaitCode            33
#define T_SerialProtocol          34
#define T_OFF                     35
#define T_ON                      36
#define T_PAUSE                   37
#define T_BACK                    38
#define T_Reporting               39
#define T_OpenRpt                 40
#define T_CloseRpt                41
#define T_ForceArm                42
#define T_AcFail                  43
#define T_LowCpuBat               44
#define T_SenAlrmRestore          45
#define T_24HourTamper            46
#define T_SupTamper               47
#define T_NoUsage                 48
#define T_SwingerShtDwn           49
#define T_ProgramRpt              50
#define T_FireVerify              51
#define T_Timers                  52
#define T_LatchkeyTm              53
#define T_EntryDelay              54
#define T_ExitDelay               55
#define T_NoActivityTmOut         56
#define T_AutoPhTest              57
#define T_SupTime                 58
#define T_AlarmCancel             59
#define T_RfTmOut                 60
#define T_FailToOpenTm            61
#define T_FailToCloseTm           62
#define T_SirenTmOut              63
#define T_ArmLedShutDwn           64
#define T_UnvacatedPremis         65
#define T_SupProtst               66
#define T_24HourClock             67
#define T_SiaLimits               68
#define T_LineCutDetect           69
#define T_NxtPhTst                70
#define T_SmkSupervision          71
#define T_ReArmTimer              72
#define T_TouchpadOptions         73
#define T_KeyfobNoDelay           74
#define T_PanicAlarms             75
#define T_RemoteTpArm             76
#define T_SystemOptions           77
#define T_RfJamDetect             78
#define T_DemoMode                79
#define T_EuroCompliance          80
#define T_HW1Function             81
#define T_HW2Function             82
#define T_SirenOptions            83
#define T_PiezoBeeps              84
#define T_PanelVoice              85
#define T_PanelPiezoAlrm          86
#define T_TroubleBeeps            87
#define T_VoiceChime              88
#define T_StatusBeepVol           89
#define T_HwSirenSup              90
#define T_SpeakerVolume           91
#define T_ExtSirenDelay           92
#define T_SilentPolPanic          93
#define T_PanelTamprAlrm          94
#define T_AlarmRptVerify          95
#define T_2wayRfTpVoice           96
#define T_SensorChime             97
#define T_SensorInfoOK            98
#define T_CommTestOn              99
#define T_AM                      100
#define T_PM                      101
#define T_LghtCntrl               102
#define T_EntryLght               103
#define T_SensorMap               104
#define T_StarForNextAlarm        105
#define T_PressLBToExit           106
#define T_HouseCd                 107
#define T_RndsDetected            108
#define T_EntryLghtData           109
#define T_AVM                     110
#define T_SensorTest              111
#define T_CommTest                112
#define T_SystemDwnLoad           113
#define T_firePhrase              114
#define T_sysBatOk                115
#define T_RF_IS_OK                116
#define T_SystemTamper            117
#define T_SYSTEMPHONEOK           118
#define T_VoxRxGain               119
#define T_VoxMicGain              120
#define T_VoxMicGainRnge          121
#define T_ManMicGain              122
#define T_AudioMode               123
#define T_FireShtDwn              124
#define T_PanicTalk               125
#define T_DwnLdrCode              126
#define T_Account                 127
#define T_SensorComplete          128
#define T_PhLock                  129
#define T_AutoArm                 130
#define T_ExitExtend              131
#define T_SecureArming            132
#define T_NoArmOnLBAt             133
#define T_QuikExit                134
#define T_DwnLdrEnable            135
#define T_Phones                  136
#define T_Phone1                  137
#define T_Phone2                  138
#define T_Phone3                  139
#define T_Phone4                  140
#define T_DwnLdrPh                141
#define T_RptOpts                 142
#define T_RptCommModes            143
#define T_Ph1Mode                 144
#define T_Ph2Mode                 145
#define T_Ph3Mode                 146
#define T_Ph4Mode                 147
#define T_INet1Mode               148
#define T_INet2Mode               149
#define T_Dtim1Mode               150
#define T_Dtim2Mode               151
#define T_AllSIA                  152
#define T_ALLCID                  153
#define T_AlrmsSIA                154
#define T_AlrmsCID                155
#define T_NonAlrmsSIA             156
#define T_NonAlrmsCID             157
#define T_BackupSIA               158
#define T_BackupCID               159
#define T_VoiceDialer             160
#define T_ModeOff                 161
#define T_CodeLen                 162
#define T_DealerCode              163
#define T_InstallerCode           164
#define T_MasterCode              165
#define T_UserCode1               166
#define T_UserCode2               167
#define T_UserCode3               168
#define T_UserCode4               169
#define T_UserCode5               170
#define T_UserCode6               171
#define T_UserCode7               172
#define T_UserCode8               173
#define T_DuressCode              174
#define T_ManagerCode             175
#define T_LkOutTm                 176
#define T_PRESSSTARPHRASE         177
#define T_EditSensor              178
#define T_LearnSensor             179
#define T_DeleteSensor            180
#define T_SensorInfo              181
#define T_TripSensorName          182
#define T_EditName                183
#define T_DelZone                 184
#define T_Deleted                 185
#define T_Disarmed                186
#define T_DrWindowOn              187
#define T_NU188                   188
#define T_MotionsOn               189
#define T_DoorWndMot              190
#define T_LatchKeyAlrm            191
#define T_AcPowerFail             192
#define T_SystemBatFail           193
#define T_ReportingFail           194
#define T_PhoneLineFail           195
#define T_NoActivityAlrmPhrase    196 
#define T_IntSirenTrbl            197
#define T_ExtSirenTrbl            198
#define T_TCPIPFailPhrase         199 
#define T_RFJam                   200
#define T_SensorAlarm             201
#define T_SensorFail              202 
#define T_LowBat                  203
#define T_SensorBypass            204
#define T_TamperSensor            205
#define T_OpenSensor              206 
#define T_SensorStatus            207 
#define T_SystemPanicAlarm        208
#define T_SystemAcessAlarm        209
#define T_QuickExit  		      210
#define T_AlarmCancel1 		      211
#define T_LatchKeyPrompt          212
#define T_NoDelayPrompt           213
#define T_Instant                 214
#define T_ChkInFault              215
#define T_AllLightsOn             216
#define T_AllLightsOff            217
#define T_IndividualLiteOn        218
#define T_IndividualLiteOff       219
#define T_EnterLiteNbr            220
#define T_Invalid                 221
#define T_NotAvailable            222
#define T_TripSensor              223
#define T_Register                224
#define T_LineNumber              225
#define T_DHCP                    226
#define T_StaticModIpAddr         227
#define T_StaticRoutIpAddr        228
#define T_StaticSubNetMsk         229
#define T_CurModIpAddr            230
#define T_CurRoutIpAddr           231
#define T_CurSubNetMsk            232
#define T_MacAddr                 233
#define T_ProxSrvrAddr            234
#define T_ProxSrvrPort            235
#define T_MainSrvrIpAddr          236
#define T_MainSrvrPort            237
#define T_PingSrvrIpAddr          238
#define T_PingSrvrPort            239
#define T_PingPgName              240
#define T_MainPgName              241
#define T_RegPgName               242
#define T_SensorTestOk            243
#define T_SensorTestFail          244
#define T_SystemOn                245
#define T_Rx1IpAddr               246
#define T_Rx1Nbr                  247               
#define T_Rx1PollPeriod           248
#define T_Rx1Port                 249
#define T_SystemActivated         250
#define T_Rx2IpAddr               251
#define T_Rx2Nbr                  252
#define T_Rx2PollPeriod           253
#define T_Rx2Port                 254
#define T_SrvrRegistered          255
#define T_LghtSched               256
#define T_SetLightSchedS          257
#define T_SilentExit              258
#define T_MemoryFull              259
#define T_ZnExistsError           260
#define T_ZnLrnFail               261
#define T_ExitProgramming         262
#define T_BeepHigh                263
#define T_SystemDone              264
#define T_IdleText                265
#define T_ArmingLvl               266
#define T_PhTestOn                267
#define T_DownLoading             268
#define T_CSDemo                  269
#define T_PhTestOk                270
#define T_PhTestFail              271
#define T_DownLoadOk              272
#define T_DownLoadFail            273
#define T_EnterDisarmCode         274
#define T_Rev                     275
#define T_SensorChime2            276
#define T_SensorChime3            277
#define T_SubDisarmed             278
#define T_SetLatchkeyTime         279
#define T_LatchKey                280
#define T_Contrast                281
#define T_SmokeWarningSensor      282
#define T_ExitError               283
#define T_FailToOpen              284
#define T_FailToClose             285
#define T_IdleTextNoArming        286
#define T_ReportAborted           287
#define T_SensorAlarmStatus       288
#define T_SensorAlarmHistory      289
#define T_SystemPanicAlarmHistory 290
#define T_SystemAcessAlarmHistory 291
#define T_BackTamper              292
#define T_LatchKeyPrompt2         293
#define T_Open                    294
#define T_Close                   295
#define T_ResetDisplayCommand     296
#define T_InteractiveServices     297
#define T_SetDate					298

#define T_Null						299
#define	T_ArmIn						300
#define T_Calibration				301
#define T_TouchEachCross			302
#define T_RequestReceived			303
#define T_EventHistory				304

#define T_Version					305
#define T_Copyright					306

#define T_CurrentTemp				307
#define T_SetTo						308
#define T_Thermostat				309
#define T_System					310
#define T_Fan						311

#define T_EnrollWait				312
#define T_EnrollReceivingdata		313
#define T_EnrollSuccess				314
#define T_EnrollFail				315

#define T_FourDayForcast			316
#define T_Updated					317
#define T_PipeChar					318
#define T_HiLo						319
#define T_PercentPrecip				320
#define T_Today						321
#define T_Sunday					322
#define T_Monday					323
#define T_Tuesay					324
#define T_Wednesday					325
#define T_Thursday					326
#define T_Friday					327
#define T_Saturday					328

#define T_TpAuxAlert				329
#define T_PanelTamper				330
#define T_TpPoliceAlert				331
#define T_TpFireAlert				332
#define T_AlarmCancelled			333
#define T_NoActivity				334
#define T_ManualPhoneTest			335
#define T_OpeningReport				336
#define T_ClosingReport				337
#define T_DuressAlarm				338
#define T_ForcedArm					339
#define T_ACpowerFailure			340
#define T_LowPanelBattery			341
#define T_AutoPhoneTest				342
#define T_ReceiverFailure			343
#define T_PhoneFailure				344
#define T_Latchkey					345
#define T_Fail2Open					346
#define T_Fail2Close				347
#define T_ExitError2				348
#define T_RecentClosing				349
#define T_WalkTestBegin				350
#define T_WalkTestEnd				351
#define T_NoUsage2					352
#define T_LineCutDetected			353
#define T_StartProgramMode			354
#define T_EndProgramMode			355
#define T_TpTamper					356
#define T_UserAccessCodeChanged		357
#define T_UserAccessCodeDeleted		358
#define T_Sirenfailure				359
#define T_IPpollingOff				360

#define T_Sn						361

#define T_Alarm						362
#define T_LowBattery				363
#define T_Supervisory				364
#define T_AlarmCanceled				365
#define T_Restoral					366
#define T_Tamper					367
#define T_FobOpenClose				368
#define T_Trouble					369
#define T_Bypassed					370
#define T_AlarmPending				371
#define T_TamperRestoral			372
#define T_SupervRestoral			373
#define T_Unbypassed				374
#define T_Null1						375
#define T_Null2						376
#define T_Null3						377

#define T_Dealer					378
#define T_Installer					379
#define T_Master					380
#define T_User1						381
#define T_User2						382
#define T_User3						383
#define T_User4						384
#define T_User5						385
#define T_User6						386
#define T_User7						387
#define T_User8						388
#define T_Duress					389

#define T_OpenDoorsList				390
#define T_OpenWindowsList			391
#define T_Motions					392
#define T_MotionDetected			393
#define T_MinutesAgo				394
#define T_NoMotionSensDetected		395
//#define T_ListItem					396

#define T_NoOpenSensors				396

#define T_Help_Btn_1_data_1         397
#define T_Help_Btn_1_data_2         398
#define T_Help_Btn_1_data_3         399
#define T_Help_Btn_1_data_4         400
#define T_Help_Btn_1_data_5         401
#define T_Help_Btn_1_data_6         402
#define T_Help_Btn_1_data_7         403
#define T_Help_Btn_1_data_8         404
#define T_Help_Btn_1_data_9         405
#define T_Help_Btn_1_data_10        406
#define T_Help_Btn_1_data_11        407
#define T_Help_Btn_1_data_12        408

#define T_Help_Btn_2_data_1         409
#define T_Help_Btn_2_data_2         410
#define T_Help_Btn_2_data_3         411
#define T_Help_Btn_2_data_4         412
#define T_Help_Btn_2_data_5         413
#define T_Help_Btn_2_data_6         414
#define T_Help_Btn_2_data_7         415
#define T_Help_Btn_2_data_8         416
#define T_Help_Btn_2_data_9         417
#define T_Help_Btn_2_data_10        418
#define T_Help_Btn_2_data_11        419
#define T_Help_Btn_2_data_12        420

#define T_Help_Btn_3_data_1         421
#define T_Help_Btn_3_data_2         422
#define T_Help_Btn_3_data_3         423
#define T_Help_Btn_3_data_4         424
#define T_Help_Btn_3_data_5         425
#define T_Help_Btn_3_data_6         426
#define T_Help_Btn_3_data_7         427
#define T_Help_Btn_3_data_8         428
#define T_Help_Btn_3_data_9         429
#define T_Help_Btn_3_data_10        430
#define T_Help_Btn_3_data_11        431
#define T_Help_Btn_3_data_12        432

#define T_Help_Btn_4_data_1         433
#define T_Help_Btn_4_data_2         434
#define T_Help_Btn_4_data_3         435
#define T_Help_Btn_4_data_4         436
#define T_Help_Btn_4_data_5         437
#define T_Help_Btn_4_data_6         438
#define T_Help_Btn_4_data_7         439
#define T_Help_Btn_4_data_8         440
#define T_Help_Btn_4_data_9         441
#define T_Help_Btn_4_data_10        442
#define T_Help_Btn_4_data_11        443
#define T_Help_Btn_4_data_12        444

#define T_Help_Btn_5_data_1         445
#define T_Help_Btn_5_data_2         446
#define T_Help_Btn_5_data_3         447
#define T_Help_Btn_5_data_4         448
#define T_Help_Btn_5_data_5         449
#define T_Help_Btn_5_data_6         450
#define T_Help_Btn_5_data_7         451
#define T_Help_Btn_5_data_8         452
#define T_Help_Btn_5_data_9         453
#define T_Help_Btn_5_data_10        454
#define T_Help_Btn_5_data_11        455
#define T_Help_Btn_5_data_12        456

#define T_Help_Btn_6_data_1         457
#define T_Help_Btn_6_data_2         458
#define T_Help_Btn_6_data_3         459
#define T_Help_Btn_6_data_4         460
#define T_Help_Btn_6_data_5         461
#define T_Help_Btn_6_data_6         462
#define T_Help_Btn_6_data_7         463
#define T_Help_Btn_6_data_8         464
#define T_Help_Btn_6_data_9         465
#define T_Help_Btn_6_data_10        466
#define T_Help_Btn_6_data_11        467
#define T_Help_Btn_6_data_12        468

#define T_Help_Btn_7_data_1         469
#define T_Help_Btn_7_data_2         470
#define T_Help_Btn_7_data_3         471
#define T_Help_Btn_7_data_4         472
#define T_Help_Btn_7_data_5         473
#define T_Help_Btn_7_data_6         474
#define T_Help_Btn_7_data_7         475
#define T_Help_Btn_7_data_8         476
#define T_Help_Btn_7_data_9         477
#define T_Help_Btn_7_data_10        478
#define T_Help_Btn_7_data_11        479
#define T_Help_Btn_7_data_12        480

#define T_Help_Btn_8_data_1         481
#define T_Help_Btn_8_data_2         482
#define T_Help_Btn_8_data_3         483
#define T_Help_Btn_8_data_4         484
#define T_Help_Btn_8_data_5         485
#define T_Help_Btn_8_data_6         486
#define T_Help_Btn_8_data_7         487
#define T_Help_Btn_8_data_8         488
#define T_Help_Btn_8_data_9         489
#define T_Help_Btn_8_data_10        490
#define T_Help_Btn_8_data_11        491
#define T_Help_Btn_8_data_12        492

#define T_LightNames                493
#define T_DoorLockNames             494
#define T_NoLights                  495
#define T_NoDoorLocks               496
#define T_RfFailure                 497

#define T_Door_1                    498
#define T_Door_2                    499
#define T_Door_3                    500
#define T_Door_4                    501
#define T_Door_Locks_Closed_Red     502

#define T_Access_Codes                  503
#define T_Dealer_Code                   504
#define T_Installer_Code                505
#define T_Master_Code                   506
#define T_User_Code_1                   507
#define T_User_Code_2                   508
#define T_User_Code_3                   509
#define T_User_Code_4                   510
#define T_User_Code_5                   511
#define T_User_Code_6                   512
#define T_User_Code_7                   513
#define T_User_Code_8                   514
#define T_Duress_Code                   515
#define T_Code_Size                     516
#define T_Audio_Verification            517
#define T_Audio_Mode                    518
#define T_Fire_Shutdown                 519
#define T_Panic_Talk                    520
#define T_VOX_Receiver_Gain             521
#define T_VOX_Microphone_Gain           522
#define T_VOX_Microphone_Gain_Range     523
#define T_Manual_Microphone_Gain        524
#define T_Fire_Enabled_AVM              525
#define T_Communication_Modes           526
#define T_Phone_1_Report_Mode           527
#define T_Phone_2_Report_Mode           528
#define T_Phone_3_Report_Mode           529
#define T_Phone_4_Report_Mode           530
#define T_Light_Control                 531
#define T_Phone_Numbers                 532
#define T_Phone_Number_1                533
#define T_Phone_Number_2                534
#define T_Phone_Number_3                535
#define T_Phone_Number_4                536
#define T_Downloader_Number             537
#define T_Phone_Options                 538
#define T_Manual_Phone_Test             539
#define T_Fail_To_Communicate           540
#define T_DTMF_Dialing                  541
#define T_300_BPS_Enabled               542
#define T_Ring_Hang_Ring                543
#define T_Dialer_Delay                  544
#define T_Call_Waiting_Code             545
#define T_Reporting_Options             546
#define T_Opening_Report                547
#define T_Closing_Report                548
#define T_Force_Armed_Report            549
#define T_AC_Power_Failure_Report       550
#define T_Panel_Low_Batt_Report         551
#define T_Sensor_Alarm_Restoral_Report  552
#define T_24_Hour_Sensor_Tamper         553
#define T_Supervisory_Tamper_Report     554
#define T_No_Usage_Report               555
#define T_Swinger_Shutdown              556
#define T_Programming_Report            557
#define T_Fire_Alarm_Verification       558
//#define T_Reporting                     559 //use #39
#define T_System_Programming            559
//#define T_Security                      561 //use #13
#define T_Account_Number                560
#define T_Downloader_Code               561
#define T_Phone_Lock                    562
#define T_Autoarm                       563
#define T_Exit_Extension                564
#define T_Secure_Arming                 565
#define T_No_Arm_On_Panel_Low_Battery   566
#define T_Quick_Exit                    567
#define T_Downloader_Enable             568
#define T_Supervisory_Protest           569
#define T_Siren_Options                 570
#define T_Panel_Piezo_Beeps             571
#define T_Panel_Voice                   572
#define T_Panel_Piezo_Alarm             573
#define T_Trouble_Beeps                 574
#define T_Voice_Chime                   575
#define T_Status_Beep_Volume            576
#define T_HW_Siren_Supervision          577
#define T_Speaker_Volume                578
#define T_Silent_Police_Panic           579
#define T_Panel_Tamper_Alarm            580
#define T_Alarm_Report_Verify           581
#define T_System_Options                582
#define T_RF_Jam_Detect                 583
#define T_Demo_Mode                     584
#define T_HW_1_Function                 585
#define T_24_Hour_Clock                 586
#define T_System_Tests                  587
//#define T_Timers                        590 //use #52
#define T_Latchkey_Time                 588
#define T_Entry_Delay                   589
#define T_Exit_Delay                    590
#define T_No_Activity_Timeout           591
#define T_Auto_Phone_Test               592
#define T_Supervisory_Time              593
#define T_Alarm_Cancel_Window           594
#define T_RF_Timeout                    595
#define T_Fail_To_Open_Time             596
#define T_Fail_To_Close_Time            597
#define T_Siren_Timeout                 598
#define T_Arm_LED_Shutdown              599
#define T_Unvacated_Premisis            600
#define T_Smoke_Supervision             601
#define T_Touchpad_Options              602
#define T_Keyfob_No_Delay               603
#define T_Panic_Alarms                  604
#define T_Remote_TP_Arming              605
#define T_Edit_Sensor_Name              606
#define T_Edit_Sensor_Group             607
#define T_Edit_Sensors                  608
#define T_Sensor_Programming            609
#define T_Edit_Sensor_Token             610
#define T_Sensor_Number                 611
#define T_Sensor_Group                  612
#define T_Sensor_Name                   613
#define T_Set_System_Time               614
#define T_Set_System_Date               615
#define T_Set_Supervisory_Time          616
#define T_Set_Fail_To_Open_Time         617
#define T_Set_Fail_To_Close_Time        618
#define T_Blank                         619
#define T_Product_Code                  620
#define T_Set_Date_Time                 621
#define T_Set_Date                      622
#define T_Image                         623
#define T_Edit_Sensor_Number            624
#define T_Set_Hours                     625
#define T_Set_Minutes                   626
#define T_Set_Day                       627
#define T_Set_Month                     628
#define T_Set_Year                      629
#define T_Edit_Inventory                630

#endif
