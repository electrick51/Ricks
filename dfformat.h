#ifndef DFFORMAT_H
#define DFFORMAT_H

/**
 * This is a generated file. Do not manually edit.
 */
#define NUMBER_OF_ZNTOKENS 199
//This following code was added from REM statements in "dfformat.ini".
//
// The following files must be the same size. The files define the storage
// space for the downloadable company images:
// 1) scracth_space.bin is used to temporily store stream images - 8k bytes
// 2) company_logo1.bin is used to store a company logo - 8000 bytes
// 3) company_logo2.bin is used to store a company logo - 8000 bytes
//
#define MAX_IMAGE_SZ 15840								//must be <= company_logo1 and scratch_space.bin size 
#define STREAM_BLOCK_SZ 132
#define MAX_BLOCK_COUNT MAX_IMAGE_SZ/STREAM_BLOCK_SZ 	//valu must be < than company_logox and scratch_space.bin size/132 
								 						//132 is the number of bytes in each rf stream block... change to define 

#define INIT_VARS_START 0x000000L        // 1x4k read only/write protected
#define UNUSED_SECTOR 0x001000L          // 1x4k read only/write protected       
#define MFG_DATA_START 0x002000L         // 1x4k read/write 
#define NV_DATA_START 0x003000L          // 1x4k read/write stores the structure nvData ~ 1442 bytes see the map file and search for nvdata
#define TESTHEAD_DATA_START 0x004000L    // 1x4k read/write  reserved for test head information
#define SPARE_SECTOR 0x005000L           // 1x4k read/write
#define DATA_START 0x006000L             // 1786x4k read only/write protected, images icons, tokens etc
#define UNUSED_SECTORS 0x700000L         // 4x4k read/write
#define EVENT_BUFFER_START 0x704000L     // 1x4k read/write  
#define AD_START 0x705000L               // 13x4k read/write
#define BANNER_START 0x712000L           // 6x4k read/write
#define PHOTOGRAPHS_START_BYTE 0x718000L // 1x4k read/write this whole page is used to store 1 byte that depicts the newest image in the next section
#define PHOTOGRAPHS_START 0x719000L      // 82x4k read/write
#define SCRATCH_SPACE_START 0x76B000L;   // 142x4k read/write.

#define COMPANY_LOGO1 0x7F9000L          // 4x4k read/write (4 sectors) CAUTION space reserved must >= MAX_IMAGE_SZ 
#define COMPANY_LOGO2 0x7FD000L          // 4x4k read/write (4 sectors) CAUTION space reserved must >= MAX_IMAGE_SZ  
   
#define HIGHEST_SFLASH_ADDRESS 0x7FFFFF; //

struct VOICEBUF
{ 
    uint8_t voice[9];
};

#define Max_Tokens (sizeof( struct VOICEBUF))

struct UISTRINGS
{
    const uint8_t *lcd;
    const struct VOICEBUF voice;
};
//
void dfformat_init(void); 
uint32_t getAdStart(uint8_t adN);
uint32_t getNVDStart(void);
uint32_t getMfgStart(void);
uint32_t getTestHeadStart(void);
uint32_t getEventbufferStart(void); 
uint32_t getBannerStart(void);
uint32_t getPhotographStart(void);
void incrementScratchPageIndex(void);
uint32_t getScratchSpaceAddress(void);
uint32_t getPhotographStartByte(void);
uint32_t getVoiceAddress(uint8_t);
uint32_t getUIStringsTextAddress(uint16_t);
uint32_t getZnTextAddress(uint16_t text_token);
uint32_t get_image_address(uint32_t image_number);
uint32_t get_image_size(uint16_t image_number);
uint32_t getVoiceTokenStartAddress(uint16_t phraseN);
uint32_t getZnVoiceTokenStartAddress(uint16_t phraseN);
uint8_t getToknToTxt(uint8_t *index,uint8_t maxIndex,uint8_t dest[],uint16_t token);
uint8_t getToknToTxt_EscapeSeq(uint8_t *index,uint8_t maxIndex,uint8_t dest[],uint16_t token);
//
// End of code from REM statements in "ini file".

#define FLASH_ADDR_INIT_VARS                      0x000000
#define FLASH_ADDR_UISTRINGS                      0x06EDC2
#define FLASH_ADDR_ZNTEXT                         0x070DCD
#define FLASH_ADDR_WORD_TABLE                     0x0748BB
#define FLASH_WORD_WAVS                           0x074C7E

#define DFI_ENTS                                  0
#define DFA_ENTS                                  0x000844

#define DFI_DUMMY_4                               1
#define DFA_DUMMY_4                               0x076F02

#define DFI_COMPANY_LOGO1                         2
#define DFA_COMPANY_LOGO1                         0x077124

#define DFI_COMPANY_LOGO2                         3
#define DFA_COMPANY_LOGO2                         0x077346

#define DFI_SCRN1_IMGRF                           4
#define DFA_SCRN1_IMGRF                           0x077568

#define DFI_SCRN2_IMGSIREN                        5
#define DFA_SCRN2_IMGSIREN                        0x077B8E

#define DFI_INSPIRA_8PT                           6
#define DFA_INSPIRA_8PT                           0x0781B4

#define DFI_INSPIRA_BOLD_8PT                      7
#define DFA_INSPIRA_BOLD_8PT                      0x078853

#define DFI_INSPIRA_12PT                          8
#define DFA_INSPIRA_12PT                          0x078FC0

#define DFI_INSPIRA_BOLD_12PT                     9
#define DFA_INSPIRA_BOLD_12PT                     0x0799A9

#define DFI_INSPIRA_24PT                          10
#define DFA_INSPIRA_24PT                          0x07A3DB

#define DFI_INSPIRA_BOLD_24PT                     11
#define DFA_INSPIRA_BOLD_24PT                     0x07C065

#define DFI_BACKGROUND                            12
#define DFA_BACKGROUND                            0x07DF1B

#define DFI_BACKGROUND_MAIN_AND_2ND               13
#define DFA_BACKGROUND_MAIN_AND_2ND               0x0BDB21

#define DFI_PRECIPITATION                         14
#define DFA_PRECIPITATION                         0x0FD727

#define DFI_PROPERTY1                             15
#define DFA_PROPERTY1                             0x0FD871

#define DFI_PROPERTY2                             16
#define DFA_PROPERTY2                             0x10002B

#define DFI_PROPERTY3                             17
#define DFA_PROPERTY3                             0x102667

#define DFI_PROPERTY1B                            18
#define DFA_PROPERTY1B                            0x104CED

#define DFI_PROPERTY2B                            19
#define DFA_PROPERTY2B                            0x105743

#define DFI_PROPERTY3B                            20
#define DFA_PROPERTY3B                            0x106199

#define DFI_BTN_INVISIBLE                         21
#define DFA_BTN_INVISIBLE                         0x106BEF

#define DFI_BACKGROUND_LOWER_BEVEL                22
#define DFA_BACKGROUND_LOWER_BEVEL                0x10DC75

#define DFI_BTN_SETTINGS_COG_STATIC               23
#define DFA_BTN_SETTINGS_COG_STATIC               0x113F77

#define DFI_BTN_SETTINGS_COG_TOUCH                24
#define DFA_BTN_SETTINGS_COG_TOUCH                0x114649

#define DFI_TXT_SILENT                            25
#define DFA_TXT_SILENT                            0x114D1B

#define DFI_TXT_SETTINGS_OPTIONAL                 26
#define DFA_TXT_SETTINGS_OPTIONAL                 0x115249

#define DFI_TXT_DELAY                             27
#define DFA_TXT_DELAY                             0x115FCD

#define DFI_BTN_CANCELARM_STATIC                  28
#define DFA_BTN_CANCELARM_STATIC                  0x116553

#define DFI_BTN_CANCELARM_TOUCH                   29
#define DFA_BTN_CANCELARM_TOUCH                   0x119835

#define DFI_BTN_BYPASS_STATIC                     30
#define DFA_BTN_BYPASS_STATIC                     0x11CB17

#define DFI_BTN_BYPASS_TOUCH                      31
#define DFA_BTN_BYPASS_TOUCH                      0x12057F

#define DFI_BTN_DOORS_STATIC                      32
#define DFA_BTN_DOORS_STATIC                      0x123FE7

#define DFI_BTN_DOORS_TOUCH                       33
#define DFA_BTN_DOORS_TOUCH                       0x1275E5

#define DFI_BTN_MOTION_STATIC                     34
#define DFA_BTN_MOTION_STATIC                     0x12ABE3

#define DFI_BTN_MOTION_TOUCH                      35
#define DFA_BTN_MOTION_TOUCH                      0x12E1E1

#define DFI_BTN_ARMALL_STATIC                     36
#define DFA_BTN_ARMALL_STATIC                     0x1317DF

#define DFI_BTN_ARMALL_TOUCH                      37
#define DFA_BTN_ARMALL_TOUCH                      0x135FE5

#define DFI_WINDOWS1B                             38
#define DFA_WINDOWS1B                             0x13A7EB

#define DFI_WINDOWS2B                             39
#define DFA_WINDOWS2B                             0x13B09D

#define DFI_WINDOWS3B                             40
#define DFA_WINDOWS3B                             0x13B94F

#define DFI_DOORS1B                               41
#define DFA_DOORS1B                               0x13C201

#define DFI_DOORS2B                               42
#define DFA_DOORS2B                               0x13CAB3

#define DFI_DOORS3B                               43
#define DFA_DOORS3B                               0x13D365

#define DFI_DUMMY_1                               44
#define DFA_DUMMY_1                               0x13DC17

#define DFI_DUMMY_2                               45
#define DFA_DUMMY_2                               0x13DE39

#define DFI_OCCUPIED1B                            46
#define DFA_OCCUPIED1B                            0x13E05B

#define DFI_OCCUPIED2B                            47
#define DFA_OCCUPIED2B                            0x13E9AB

#define DFI_OCCUPIED3B                            48
#define DFA_OCCUPIED3B                            0x13F2FB

#define DFI_TXT_ARM_SYSTEM                        49
#define DFA_TXT_ARM_SYSTEM                        0x13FC11

#define DFI_OR                                    50
#define DFA_OR                                    0x1405D7

#define DFI_BTN_ON_PREARM_STATIC                  51
#define DFA_BTN_ON_PREARM_STATIC                  0x1412E1

#define DFI_BTN_ON_PREARM_TOUCH                   52
#define DFA_BTN_ON_PREARM_TOUCH                   0x141C27

#define DFI_BTN_OFF_PREARM_STATIC                 53
#define DFA_BTN_OFF_PREARM_STATIC                 0x14256D

#define DFI_BTN_OFF_PREARM_TOUCH                  54
#define DFA_BTN_OFF_PREARM_TOUCH                  0x142EF3

#define DFI_BTN_ALERT_STATIC                      55
#define DFA_BTN_ALERT_STATIC                      0x143879

#define DFI_NORFCONNECTION                        56
#define DFA_NORFCONNECTION                        0x1567AF

#define DFI_CODE_FIELD_BULLET                     57
#define DFA_CODE_FIELD_BULLET                     0x164431

#define DFI_CODE_FIELD                            58
#define DFA_CODE_FIELD                            0x1645A2

#define DFI_KEYPAD_0_STATIC                       59
#define DFA_KEYPAD_0_STATIC                       0x1678E8

#define DFI_KEYPAD_0_TOUCH                        60
#define DFA_KEYPAD_0_TOUCH                        0x168BAE

#define DFI_KEYPAD_1_STATIC                       61
#define DFA_KEYPAD_1_STATIC                       0x169E74

#define DFI_KEYPAD_1_TOUCH                        62
#define DFA_KEYPAD_1_TOUCH                        0x16B13A

#define DFI_KEYPAD_2_STATIC                       63
#define DFA_KEYPAD_2_STATIC                       0x16C400

#define DFI_KEYPAD_2_TOUCH                        64
#define DFA_KEYPAD_2_TOUCH                        0x16D6C6

#define DFI_KEYPAD_3_STATIC                       65
#define DFA_KEYPAD_3_STATIC                       0x16E98C

#define DFI_KEYPAD_3_TOUCH                        66
#define DFA_KEYPAD_3_TOUCH                        0x16FC52

#define DFI_KEYPAD_4_STATIC                       67
#define DFA_KEYPAD_4_STATIC                       0x170F18

#define DFI_KEYPAD_4_TOUCH                        68
#define DFA_KEYPAD_4_TOUCH                        0x1721DE

#define DFI_KEYPAD_5_STATIC                       69
#define DFA_KEYPAD_5_STATIC                       0x1734A4

#define DFI_KEYPAD_5_TOUCH                        70
#define DFA_KEYPAD_5_TOUCH                        0x17476A

#define DFI_KEYPAD_6_STATIC                       71
#define DFA_KEYPAD_6_STATIC                       0x175A30

#define DFI_KEYPAD_6_TOUCH                        72
#define DFA_KEYPAD_6_TOUCH                        0x176CF6

#define DFI_KEYPAD_7_STATIC                       73
#define DFA_KEYPAD_7_STATIC                       0x177FBC

#define DFI_KEYPAD_7_TOUCH                        74
#define DFA_KEYPAD_7_TOUCH                        0x179282

#define DFI_KEYPAD_8_STATIC                       75
#define DFA_KEYPAD_8_STATIC                       0x17A548

#define DFI_KEYPAD_8_TOUCH                        76
#define DFA_KEYPAD_8_TOUCH                        0x17B80E

#define DFI_KEYPAD_9_STATIC                       77
#define DFA_KEYPAD_9_STATIC                       0x17CAD4

#define DFI_KEYPAD_9_TOUCH                        78
#define DFA_KEYPAD_9_TOUCH                        0x17DD9A

#define DFI_KEYPAD_POUND_STATIC                   79
#define DFA_KEYPAD_POUND_STATIC                   0x17F060

#define DFI_KEYPAD_POUND_TOUCH                    80
#define DFA_KEYPAD_POUND_TOUCH                    0x180326

#define DFI_KEYPAD_STAR_STATIC                    81
#define DFA_KEYPAD_STAR_STATIC                    0x1815EC

#define DFI_KEYPAD_STAR_TOUCH                     82
#define DFA_KEYPAD_STAR_TOUCH                     0x1828B2

#define DFI_TXT_ACCESS_CODE                       83
#define DFA_TXT_ACCESS_CODE                       0x183B78

#define DFI_BTN_CLEAR_STATIC                      84
#define DFA_BTN_CLEAR_STATIC                      0x1848E6

#define DFI_BTN_CLEAR_TOUCH                       85
#define DFA_BTN_CLEAR_TOUCH                       0x1872B0

#define DFI_BTN_SERVICE_STATIC                    86
#define DFA_BTN_SERVICE_STATIC                    0x189C7A

#define DFI_BTN_SERVICE_TOUCH                     87
#define DFA_BTN_SERVICE_TOUCH                     0x18D4C0

#define DFI_BTN_BILLING_STATIC                    88
#define DFA_BTN_BILLING_STATIC                    0x190D06

#define DFI_BTN_BILLING_TOUCH                     89
#define DFA_BTN_BILLING_TOUCH                     0x1930B6

#define DFI_TXT_INFORMATION                       90
#define DFA_TXT_INFORMATION                       0x195466

#define DFI_BTN_CONTACTUS_STATIC                  91
#define DFA_BTN_CONTACTUS_STATIC                  0x195F4A

#define DFI_BTN_CONTACTUS_TOUCH                   92
#define DFA_BTN_CONTACTUS_TOUCH                   0x199290

#define DFI_BTN_SECURITYFAQS_STATIC               93
#define DFA_BTN_SECURITYFAQS_STATIC               0x19C5D6

#define DFI_BTN_SECURITYFAQS_TOUCH                94
#define DFA_BTN_SECURITYFAQS_TOUCH                0x19F91C

#define DFI_TXT_SECURITY_FAQS                     95
#define DFA_TXT_SECURITY_FAQS                     0x1A2C62

#define DFI_BTN_SYSTEMHELP_STATIC                 96
#define DFA_BTN_SYSTEMHELP_STATIC                 0x1A4153

#define DFI_BTN_SYSTEMHELP_TOUCH                  97
#define DFA_BTN_SYSTEMHELP_TOUCH                  0x1A7499

#define DFI_BTN_TECHSUPPORT_STATIC                98
#define DFA_BTN_TECHSUPPORT_STATIC                0x1AA7DF

#define DFI_BTN_TECHSUPPORT_TOUCH                 99
#define DFA_BTN_TECHSUPPORT_TOUCH                 0x1ACB8F

#define DFI_BTN_CUSTOMERSERVICE_STATIC            100
#define DFA_BTN_CUSTOMERSERVICE_STATIC            0x1AEF3F

#define DFI_BTN_CUSTOMERSERVICE_TOUCH             101
#define DFA_BTN_CUSTOMERSERVICE_TOUCH             0x1B12EF

#define DFI_BTN_HELP_1_STATIC                     102
#define DFA_BTN_HELP_1_STATIC                     0x1B369F

#define DFI_BTN_HELP_1_TOUCH                      103
#define DFA_BTN_HELP_1_TOUCH                      0x1B9AB9

#define DFI_BTN_HELP_2_STATIC                     104
#define DFA_BTN_HELP_2_STATIC                     0x1BFED3

#define DFI_BTN_HELP_2_TOUCH                      105
#define DFA_BTN_HELP_2_TOUCH                      0x1C62ED

#define DFI_BTN_HELP_3_STATIC                     106
#define DFA_BTN_HELP_3_STATIC                     0x1CC707

#define DFI_BTN_HELP_3_TOUCH                      107
#define DFA_BTN_HELP_3_TOUCH                      0x1D2B21

#define DFI_BTN_HELP_4_STATIC                     108
#define DFA_BTN_HELP_4_STATIC                     0x1D8F3B

#define DFI_BTN_HELP_4_TOUCH                      109
#define DFA_BTN_HELP_4_TOUCH                      0x1DF355

#define DFI_BTN_HELP_5_STATIC                     110
#define DFA_BTN_HELP_5_STATIC                     0x1E576F

#define DFI_BTN_HELP_5_TOUCH                      111
#define DFA_BTN_HELP_5_TOUCH                      0x1EBB89

#define DFI_BTN_HELP_6_STATIC                     112
#define DFA_BTN_HELP_6_STATIC                     0x1F1FA3

#define DFI_BTN_HELP_6_TOUCH                      113
#define DFA_BTN_HELP_6_TOUCH                      0x1F83BD

#define DFI_BTN_REQSENT_STATIC                    114
#define DFA_BTN_REQSENT_STATIC                    0x1FE7D7

#define DFI_BTN_REQSENT_TOUCH                     115
#define DFA_BTN_REQSENT_TOUCH                     0x205A6D

#define DFI_TXT_CONTACTUS                         116
#define DFA_TXT_CONTACTUS                         0x20D273

#define DFI_TXT_SYSTEMHELP_TITLE                  117
#define DFA_TXT_SYSTEMHELP_TITLE                  0x20DE2D

#define DFI_TXT_MINUTESAGO                        118
#define DFA_TXT_MINUTESAGO                        0x20E875

#define DFI_TXT_DOORS_DISCLAIMER                  119
#define DFA_TXT_DOORS_DISCLAIMER                  0x20EF53

#define DFI_TXT_OPENDOORS_TITLE                   120
#define DFA_TXT_OPENDOORS_TITLE                   0x210241

#define DFI_TXT_ZONES_DISCLAIMER                  121
#define DFA_TXT_ZONES_DISCLAIMER                  0x210C89

#define DFI_TXT_MOTIONACTIVITY_TITLE              122
#define DFA_TXT_MOTIONACTIVITY_TITLE              0x211BAF

#define DFI_TXT_WINDOWS_DISCLAIMER                123
#define DFA_TXT_WINDOWS_DISCLAIMER                0x212985

#define DFI_TXT_OPENWINDOWS_TITLE                 124
#define DFA_TXT_OPENWINDOWS_TITLE                 0x213DA7

#define DFI_TXT_PROPERTY_DISCLAIMER               125
#define DFA_TXT_PROPERTY_DISCLAIMER               0x214A79

#define DFI_TXT_PROPERTYSTATUS_TITLE              126
#define DFA_TXT_PROPERTYSTATUS_TITLE              0x215E85

#define DFI_TXT_ACTIVITY                          127
#define DFA_TXT_ACTIVITY                          0x216C75

#define DFI_TXT_OPENACTIVATED                     128
#define DFA_TXT_OPENACTIVATED                     0x2177BB

#define DFI_CLEAR_THERMO_DISPLAY                  129
#define DFA_CLEAR_THERMO_DISPLAY                  0x218ED1

#define DFI_WINDOWS1                              130
#define DFA_WINDOWS1                              0x219427

#define DFI_WINDOWS2                              131
#define DFA_WINDOWS2                              0x21B42B

#define DFI_WINDOWS3                              132
#define DFA_WINDOWS3                              0x21D3A5

#define DFI_DOORS1                                133
#define DFA_DOORS1                                0x21F3A9

#define DFI_DOORS2                                134
#define DFA_DOORS2                                0x2214AF

#define DFI_DOORS3                                135
#define DFA_DOORS3                                0x223529

#define DFI_ARMED1                                136
#define DFA_ARMED1                                0x22562F

#define DFI_ARMED2                                137
#define DFA_ARMED2                                0x22B107

#define DFI_OCCUPIED1                             138
#define DFA_OCCUPIED1                             0x230BDF

#define DFI_OCCUPIED2                             139
#define DFA_OCCUPIED2                             0x2330B5

#define DFI_OCCUPIED3                             140
#define DFA_OCCUPIED3                             0x235233

#define DFI_BTN_THERMO_STATIC                     141
#define DFA_BTN_THERMO_STATIC                     0x237709

#define DFI_BTN_THERMO_TOUCH                      142
#define DFA_BTN_THERMO_TOUCH                      0x23986B

#define DFI_TXT_INSIDE                            143
#define DFA_TXT_INSIDE                            0x23B9CD

#define DFI_TXT_OUTSIDE                           144
#define DFA_TXT_OUTSIDE                           0x23BFF3

#define DFI_TXT_DISARM_IN                         145
#define DFA_TXT_DISARM_IN                         0x23C701

#define DFI_BANNER_BG                             146
#define DFA_BANNER_BG                             0x23CF5B

#define DFI_BANNER_BG_W_TXT                       147
#define DFA_BANNER_BG_W_TXT                       0x23FCA7

#define DFI_FEW                                   148
#define DFA_FEW                                   0x240797

#define DFI_MCLOUDY                               149
#define DFA_MCLOUDY                               0x243113

#define DFI_OVC                                   150
#define DFA_OVC                                   0x245A8F

#define DFI_SHRA                                  151
#define DFA_SHRA                                  0x24840B

#define DFI_SKC                                   152
#define DFA_SKC                                   0x24AD87

#define DFI_SN                                    153
#define DFA_SN                                    0x24D703

#define DFI_TSRA                                  154
#define DFA_TSRA                                  0x25007F

#define DFI_FOG                                   155
#define DFA_FOG                                   0x2529FB

#define DFI_NBKN                                  156
#define DFA_NBKN                                  0x255377

#define DFI_NFG                                   157
#define DFA_NFG                                   0x257CF3

#define DFI_NOVC                                  158
#define DFA_NOVC                                  0x25A66F

#define DFI_NRA                                   159
#define DFA_NRA                                   0x25CFEB

#define DFI_NSKC                                  160
#define DFA_NSKC                                  0x25F967

#define DFI_NSN                                   161
#define DFA_NSN                                   0x2622E3

#define DFI_NTSRA                                 162
#define DFA_NTSRA                                 0x264C5F

#define DFI_PCLOUDYN                              163
#define DFA_PCLOUDYN                              0x2675DB

#define DFI_RAINDROP                              164
#define DFA_RAINDROP                              0x269F57

#define DFI_WIND2                                 165
#define DFA_WIND2                                 0x26A179

#define DFI_NHVYRA                                166
#define DFA_NHVYRA                                0x26CAF5

#define DFI_PINNACLE_LOGO                         167
#define DFA_PINNACLE_LOGO                         0x26F696

#define DFI_INTERLOGIX_LOGO                       168
#define DFA_INTERLOGIX_LOGO                       0x270A76

#define DFI_DEFENDER_LOGO                         169
#define DFA_DEFENDER_LOGO                         0x273074

#define DFI_TXT_THERMOSTAT                        170
#define DFA_TXT_THERMOSTAT                        0x2746FA

#define DFI_TXT_SYSTEM                            171
#define DFA_TXT_SYSTEM                            0x275180

#define DFI_TXT_SETTO                             172
#define DFA_TXT_SETTO                             0x27579E

#define DFI_TXT_FAN                               173
#define DFA_TXT_FAN                               0x275C84

#define DFI_TXT_CURRENT                           174
#define DFA_TXT_CURRENT                           0x275EE2

#define DFI_BTN_THERMO1_STATIC                    175
#define DFA_BTN_THERMO1_STATIC                    0x276410

#define DFI_BTN_THERMO1_TOUCH                     176
#define DFA_BTN_THERMO1_TOUCH                     0x2773E8

#define DFI_BTN_THERMO2_STATIC                    177
#define DFA_BTN_THERMO2_STATIC                    0x2783C0

#define DFI_BTN_THERMO2_TOUCH                     178
#define DFA_BTN_THERMO2_TOUCH                     0x279398

#define DFI_BTN_THERMO3_STATIC                    179
#define DFA_BTN_THERMO3_STATIC                    0x27A370

#define DFI_BTN_THERMO3_TOUCH                     180
#define DFA_BTN_THERMO3_TOUCH                     0x27B348

#define DFI_BTN_TEMPUP_STATIC                     181
#define DFA_BTN_TEMPUP_STATIC                     0x27C320

#define DFI_BTN_TEMPUP_TOUCH                      182
#define DFA_BTN_TEMPUP_TOUCH                      0x27D434

#define DFI_BTN_TEMPDOWN_STATIC                   183
#define DFA_BTN_TEMPDOWN_STATIC                   0x27E548

#define DFI_BTN_TEMPDOWN_TOUCH                    184
#define DFA_BTN_TEMPDOWN_TOUCH                    0x27F65C

#define DFI_BTN_RUN_STATIC                        185
#define DFA_BTN_RUN_STATIC                        0x280770

#define DFI_BTN_RUN_TOUCH                         186
#define DFA_BTN_RUN_TOUCH                         0x281884

#define DFI_BTN_ON_STATIC                         187
#define DFA_BTN_ON_STATIC                         0x282998

#define DFI_BTN_ON_TOUCH                          188
#define DFA_BTN_ON_TOUCH                          0x283AAC

#define DFI_BTN_OFF_STATIC                        189
#define DFA_BTN_OFF_STATIC                        0x284BC0

#define DFI_BTN_OFF_TOUCH                         190
#define DFA_BTN_OFF_TOUCH                         0x285CD4

#define DFI_BTN_HOLD_STATIC                       191
#define DFA_BTN_HOLD_STATIC                       0x286DE8

#define DFI_BTN_HOLD_TOUCH                        192
#define DFA_BTN_HOLD_TOUCH                        0x287EFC

#define DFI_BTN_HEAT_STATIC                       193
#define DFA_BTN_HEAT_STATIC                       0x289010

#define DFI_BTN_HEAT_TOUCH                        194
#define DFA_BTN_HEAT_TOUCH                        0x28A124

#define DFI_BTN_AUTO_STATIC                       195
#define DFA_BTN_AUTO_STATIC                       0x28B238

#define DFI_BTN_AUTO_TOUCH                        196
#define DFA_BTN_AUTO_TOUCH                        0x28C34C

#define DFI_BTN_AC_STATIC                         197
#define DFA_BTN_AC_STATIC                         0x28D460

#define DFI_BTN_AC_TOUCH                          198
#define DFA_BTN_AC_TOUCH                          0x28E574

#define DFI_TXT_FORECAST                          199
#define DFA_TXT_FORECAST                          0x28F688

#define DFI_TXT_TODAY                             200
#define DFA_TXT_TODAY                             0x28FE60

#define DFI_TXT_SUNDAY                            201
#define DFA_TXT_SUNDAY                            0x2905C8

#define DFI_TXT_MONDAY                            202
#define DFA_TXT_MONDAY                            0x290D5A

#define DFI_TXT_TUESDAY                           203
#define DFA_TXT_TUESDAY                           0x29156A

#define DFI_TXT_WEDNESDAY                         204
#define DFA_TXT_WEDNESDAY                         0x291DCE

#define DFI_TXT_THURSDAY                          205
#define DFA_TXT_THURSDAY                          0x2929A4

#define DFI_TXT_FRIDAY                            206
#define DFA_TXT_FRIDAY                            0x29332E

#define DFI_TXT_SATURDAY                          207
#define DFA_TXT_SATURDAY                          0x293970

#define DFI_HEADER_SECONDARY                      208
#define DFA_HEADER_SECONDARY                      0x29427C

#define DFI_HEADER_HOME                           209
#define DFA_HEADER_HOME                           0x29DFE4

#define DFI_BTN_SCROLL_UP_STATIC                  210
#define DFA_BTN_SCROLL_UP_STATIC                  0x2A9F2A

#define DFI_BTN_SCROLL_UP_TOUCH                   211
#define DFA_BTN_SCROLL_UP_TOUCH                   0x2AB1EA

#define DFI_BTN_SCROLL_DOWN_STATIC                212
#define DFA_BTN_SCROLL_DOWN_STATIC                0x2AC4AA

#define DFI_BTN_SCROLL_DOWN_TOUCH                 213
#define DFA_BTN_SCROLL_DOWN_TOUCH                 0x2AD76A

#define DFI_BTN_EMERGENCY_STATIC                  214
#define DFA_BTN_EMERGENCY_STATIC                  0x2AEA2A

#define DFI_BTN_EMERGENCY_TOUCH                   215
#define DFA_BTN_EMERGENCY_TOUCH                   0x2B07B8

#define DFI_BTN_CLOSE_STATIC                      216
#define DFA_BTN_CLOSE_STATIC                      0x2B210E

#define DFI_BTN_CLOSE_TOUCH                       217
#define DFA_BTN_CLOSE_TOUCH                       0x2B3A54

#define DFI_BTN_CANCEL_STATIC                     218
#define DFA_BTN_CANCEL_STATIC                     0x2B5072

#define DFI_BTN_CANCEL_TOUCH                      219
#define DFA_BTN_CANCEL_TOUCH                      0x2B69B8

#define DFI_BTN_OK_STATIC                         220
#define DFA_BTN_OK_STATIC                         0x2B82FE

#define DFI_BTN_OK_TOUCH                          221
#define DFA_BTN_OK_TOUCH                          0x2B9C04

#define DFI_BTN_ON_STATUS_STATIC                  222
#define DFA_BTN_ON_STATUS_STATIC                  0x2BB1EA

#define DFI_BTN_ON_STATUS_TOUCH                   223
#define DFA_BTN_ON_STATUS_TOUCH                   0x2BC8A6

#define DFI_BTN_OFF_STATUS_STATIC                 224
#define DFA_BTN_OFF_STATUS_STATIC                 0x2BDF62

#define DFI_BTN_OFF_STATUS_TOUCH                  225
#define DFA_BTN_OFF_STATUS_TOUCH                  0x2BF8FD

#define DFI_TXT_CODEREQUIRED                      226
#define DFA_TXT_CODEREQUIRED                      0x2C1298

#define DFI_BTN_YES_STATIC                        227
#define DFA_BTN_YES_STATIC                        0x2C35F6

#define DFI_BTN_YES_TOUCH                         228
#define DFA_BTN_YES_TOUCH                         0x2C4F91

#define DFI_BTN_NO_STATIC                         229
#define DFA_BTN_NO_STATIC                         0x2C692C

#define DFI_BTN_NO_TOUCH                          230
#define DFA_BTN_NO_TOUCH                          0x2C82C7

#define DFI_BTN_CHECK_STATIC                      231
#define DFA_BTN_CHECK_STATIC                      0x2C9C62

#define DFI_BTN_CHECK_TOUCH                       232
#define DFA_BTN_CHECK_TOUCH                       0x2CB5FD

#define DFI_BTN_CONTROL_STATIC                    233
#define DFA_BTN_CONTROL_STATIC                    0x2CCF98

#define DFI_BTN_CONTROL_TOUCH                     234
#define DFA_BTN_CONTROL_TOUCH                     0x2CFC3E

#define DFI_BTN_SET_STATIC                        235
#define DFA_BTN_SET_STATIC                        0x2D28E4

#define DFI_BTN_SET_TOUCH                         236
#define DFA_BTN_SET_TOUCH                         0x2D427F

#define DFI_BARS_00                               237
#define DFA_BARS_00                               0x2D5C1A

#define DFI_BARS_01                               238
#define DFA_BARS_01                               0x2D65E0

#define DFI_BARS_02                               239
#define DFA_BARS_02                               0x2D6FA6

#define DFI_BARS_03                               240
#define DFA_BARS_03                               0x2D796C

#define DFI_BARS_04                               241
#define DFA_BARS_04                               0x2D8332

#define DFI_BTN_BLANK_STATIC                      242
#define DFA_BTN_BLANK_STATIC                      0x2D8CF8

#define DFI_BTN_BLANK_TOUCH                       243
#define DFA_BTN_BLANK_TOUCH                       0x2DA693

#define DFI_BTN_ENROLL_STATIC                     244
#define DFA_BTN_ENROLL_STATIC                     0x2DC02E

#define DFI_BTN_ENROLL_TOUCH                      245
#define DFA_BTN_ENROLL_TOUCH                      0x2DE19A

#define DFI_BTN_HEAR_STATIC                       246
#define DFA_BTN_HEAR_STATIC                       0x2E0306

#define DFI_BTN_HEAR_TOUCH                        247
#define DFA_BTN_HEAR_TOUCH                        0x2E1CA1

#define DFI_BTN_LEFT_STATIC                       248
#define DFA_BTN_LEFT_STATIC                       0x2E363C

#define DFI_BTN_LEFT_TOUCH                        249
#define DFA_BTN_LEFT_TOUCH                        0x2E4FD7

#define DFI_BTN_MAIN_STATIC                       250
#define DFA_BTN_MAIN_STATIC                       0x2E6972

#define DFI_BTN_MAIN_TOUCH                        251
#define DFA_BTN_MAIN_TOUCH                        0x2E830D

#define DFI_BTN_RIGHT_STATIC                      252
#define DFA_BTN_RIGHT_STATIC                      0x2E9CA8

#define DFI_BTN_RIGHT_TOUCH                       253
#define DFA_BTN_RIGHT_TOUCH                       0x2EB643

#define DFI_BTN_SHOW_STATIC                       254
#define DFA_BTN_SHOW_STATIC                       0x2ECFDE

#define DFI_BTN_SHOW_TOUCH                        255
#define DFA_BTN_SHOW_TOUCH                        0x2EE979

#define DFI_TXT_BRIGHTNESS                        256
#define DFA_TXT_BRIGHTNESS                        0x2F0314

#define DFI_TXT_CALIBRATION                       257
#define DFA_TXT_CALIBRATION                       0x2F115A

#define DFI_TXT_CHIME                             258
#define DFA_TXT_CHIME                             0x2F1D90

#define DFI_TXT_DEFAULT                           259
#define DFA_TXT_DEFAULT                           0x2F244A

#define DFI_TXT_INSTALLER                         260
#define DFA_TXT_INSTALLER                         0x2F344F

#define DFI_TXT_LIGHTS                            261
#define DFA_TXT_LIGHTS                            0x2F4C55

#define DFI_TXT_LOCKS                             262
#define DFA_TXT_LOCKS                             0x2F546B

#define DFI_TXT_SIMONXT                           263
#define DFA_TXT_SIMONXT                           0x2F5F42

#define DFI_TXT_STATUS                            264
#define DFA_TXT_STATUS                            0x2F6CD7

#define DFI_TXT_TOUCHSCREEN                       265
#define DFA_TXT_TOUCHSCREEN                       0x2F8369

#define DFI_TXT_VOLUME                            266
#define DFA_TXT_VOLUME                            0x2F914C

#define DFI_TXT_EVENT                             267
#define DFA_TXT_EVENT                             0x2FA2B6

#define DFI_AC_01                                 268
#define DFA_AC_01                                 0x2FB3C9

#define DFI_AC_02                                 269
#define DFA_AC_02                                 0x2FB57F

#define DFI_RFOK                                  270
#define DFA_RFOK                                  0x2FB9BD

#define DFI_RFFAULT                               271
#define DFA_RFFAULT                               0x2FBDFB

#define DFI_BATTERY_01                            272
#define DFA_BATTERY_01                            0x2FC239

#define DFI_BATTERY_04                            273
#define DFA_BATTERY_04                            0x2FC677

#define DFI_BTN_CLEARHISTORY_STATIC               274
#define DFA_BTN_CLEARHISTORY_STATIC               0x2FCAB5

#define DFI_BTN_CLEARHISTORY_TOUCH                275
#define DFA_BTN_CLEARHISTORY_TOUCH                0x2FE450

#define DFI_TXT_BLANK                             276
#define DFA_TXT_BLANK                             0x2FFDEB

#define DFI_TXT_LIGHTS_TITLE                      277
#define DFA_TXT_LIGHTS_TITLE                      0x300EFE

#define DFI_TXT_VOLUMEBEEP                        278
#define DFA_TXT_VOLUMEBEEP                        0x301466

#define DFI_BTN_LOCK_STATIC                       279
#define DFA_BTN_LOCK_STATIC                       0x3025D0

#define DFI_BTN_LOCK_TOUCH                        280
#define DFA_BTN_LOCK_TOUCH                        0x303B09

#define DFI_BTN_LOCKALL_STATIC                    281
#define DFA_BTN_LOCKALL_STATIC                    0x305042

#define DFI_BTN_LOCKALL_TOUCH                     282
#define DFA_BTN_LOCKALL_TOUCH                     0x306A56

#define DFI_BTN_UNLOCK_STATIC                     283
#define DFA_BTN_UNLOCK_STATIC                     0x30846A

#define DFI_BTN_UNLOCK_TOUCH                      284
#define DFA_BTN_UNLOCK_TOUCH                      0x3099A3

#define DFI_BTN_UNLOCKALL_STATIC                  285
#define DFA_BTN_UNLOCKALL_STATIC                  0x30AEDC

#define DFI_BTN_UNLOCKALL_TOUCH                   286
#define DFA_BTN_UNLOCKALL_TOUCH                   0x30CA86

#define DFI_BTN_ALLON_STATIC                      287
#define DFA_BTN_ALLON_STATIC                      0x30E630

#define DFI_BTN_ALLON_TOUCH                       288
#define DFA_BTN_ALLON_TOUCH                       0x310166

#define DFI_BTN_ALLOFF_STATIC                     289
#define DFA_BTN_ALLOFF_STATIC                     0x311C9C

#define DFI_BTN_ALLOFF_TOUCH                      290
#define DFA_BTN_ALLOFF_TOUCH                      0x3137D2

#define DFI_BTN_LIGHTOFF_STATIC                   291
#define DFA_BTN_LIGHTOFF_STATIC                   0x315308

#define DFI_BTN_LIGHTOFF_TOUCH                    292
#define DFA_BTN_LIGHTOFF_TOUCH                    0x31687D

#define DFI_BTN_LIGHTON_STATIC                    293
#define DFA_BTN_LIGHTON_STATIC                    0x317DF2

#define DFI_BTN_LIGHTON_TOUCH                     294
#define DFA_BTN_LIGHTON_TOUCH                     0x319367

#define DFI_BTN_HELP_STATIC                       295
#define DFA_BTN_HELP_STATIC                       0x31A8DC

#define DFI_BTN_HELP_TOUCH                        296
#define DFA_BTN_HELP_TOUCH                        0x31C277

#define DFI_BTN_TEST_STATIC                       297
#define DFA_BTN_TEST_STATIC                       0x31DC12

#define DFI_BTN_TEST_TOUCH                        298
#define DFA_BTN_TEST_TOUCH                        0x31F5AD

#define DFI_TXT_HELPSCREEN                        299
#define DFA_TXT_HELPSCREEN                        0x320F48

#define DFI_TXT_RFTEST                            300
#define DFA_TXT_RFTEST                            0x32151E

#define DFI_TXT_AD                                301
#define DFA_TXT_AD                                0x321C74

#define DFI_TXT_DOORLOCKS                         302
#define DFA_TXT_DOORLOCKS                         0x322FD0

#define DFI_TXT_DOORLOCKS_DISCLAIMER              303
#define DFA_TXT_DOORLOCKS_DISCLAIMER              0x3239FE

#define DFI_CROSS                                 304
#define DFA_CROSS                                 0x324CD6

#define DFI_BTN_CANCEL_STATIC2                    305
#define DFA_BTN_CANCEL_STATIC2                    0x326497

#define DFI_BTN_EMERGENCY_FIRE_STATIC             306
#define DFA_BTN_EMERGENCY_FIRE_STATIC             0x328505

#define DFI_BTN_EMERGENCY_FIRE_TOUCH              307
#define DFA_BTN_EMERGENCY_FIRE_TOUCH              0x32AA55

#define DFI_BTN_EMERGENCY_POLICE_STATIC           308
#define DFA_BTN_EMERGENCY_POLICE_STATIC           0x32CFA5

#define DFI_BTN_EMERGENCY_POLICE_TOUCH            309
#define DFA_BTN_EMERGENCY_POLICE_TOUCH            0x32F5D3

#define DFI_BTN_EMERGENCY_PANIC_STATIC            310
#define DFA_BTN_EMERGENCY_PANIC_STATIC            0x331673

#define DFI_BTN_EMERGENCY_PANIC_TOUCH             311
#define DFA_BTN_EMERGENCY_PANIC_TOUCH             0x333CA1

#define DFI_OR_EMERGENCY                          312
#define DFA_OR_EMERGENCY                          0x335D41

#define DFI_TXT_EMERGENCY                         313
#define DFA_TXT_EMERGENCY                         0x3364B7

#define DFI_TXT_EVENT_HISTORY                     314
#define DFA_TXT_EVENT_HISTORY                     0x336E2F

#define DFI_TXT_PROMOTIONS                        315
#define DFA_TXT_PROMOTIONS                        0x337A31

#define DFI_TXT_MESSAGES                          316
#define DFA_TXT_MESSAGES                          0x338493

#define DFI_BTN_AUTOMOTIVE2                       317
#define DFA_BTN_AUTOMOTIVE2                       0x338C9F

#define DFI_BTN_AUTOMOTIVE                        318
#define DFA_BTN_AUTOMOTIVE                        0x339EED

#define DFI_TXT_AUTOMOTIVE                        319
#define DFA_TXT_AUTOMOTIVE                        0x33CA97

#define DFI_BTN_CELLPHONE2                        320
#define DFA_BTN_CELLPHONE2                        0x33D4C5

#define DFI_BTN_CELLPHONE                         321
#define DFA_BTN_CELLPHONE                         0x33DB1F

#define DFI_TXT_CELLPHONE                         322
#define DFA_TXT_CELLPHONE                         0x3406C9

#define DFI_BTN_ELECTRONICS2                      323
#define DFA_BTN_ELECTRONICS2                      0x34105B

#define DFI_BTN_ELECTRONICS                       324
#define DFA_BTN_ELECTRONICS                       0x341F25

#define DFI_TXT_ELECTRONICS                       325
#define DFA_TXT_ELECTRONICS                       0x344ACF

#define DFI_BTN_ENTERTAINMENT2                    326
#define DFA_BTN_ENTERTAINMENT2                    0x345565

#define DFI_BTN_ENTERTAINMENT                     327
#define DFA_BTN_ENTERTAINMENT                     0x3468A3

#define DFI_TXT_ENTERTAINMENT                     328
#define DFA_TXT_ENTERTAINMENT                     0x34944D

#define DFI_BTN_HOLIDAY2                          329
#define DFA_BTN_HOLIDAY2                          0x349F3D

#define DFI_BTN_HOLIDAY                           330
#define DFA_BTN_HOLIDAY                           0x34B116

#define DFI_TXT_HOLIDAY                           331
#define DFA_TXT_HOLIDAY                           0x34DCC0

#define DFI_BTN_HOME2                             332
#define DFA_BTN_HOME2                             0x34E37A

#define DFI_BTN_HOME                              333
#define DFA_BTN_HOME                              0x34F730

#define DFI_TXT_HOME                              334
#define DFA_TXT_HOME                              0x3522DA

#define DFI_BTN_ONLINE2                           335
#define DFA_BTN_ONLINE2                           0x352758

#define DFI_BTN_ONLINE                            336
#define DFA_BTN_ONLINE                            0x353256

#define DFI_TXT_ONLINE                            337
#define DFA_TXT_ONLINE                            0x355E00

#define DFI_BTN_PROMOTIONS_STATIC                 338
#define DFA_BTN_PROMOTIONS_STATIC                 0x356452

#define DFI_BTN_PROMOTIONS_TOUCH                  339
#define DFA_BTN_PROMOTIONS_TOUCH                  0x35A484

#define DFI_BTN_OTHER2                            340
#define DFA_BTN_OTHER2                            0x35E4B6

#define DFI_BTN_OTHER                             341
#define DFA_BTN_OTHER                             0x35F3EC

#define DFI_TXT_OTHER                             342
#define DFA_TXT_OTHER                             0x361F96

#define DFI_BTN_RESTAURANT2                       343
#define DFA_BTN_RESTAURANT2                       0x3624B0

#define DFI_BTN_RESTAURANT                        344
#define DFA_BTN_RESTAURANT                        0x36351E

#define DFI_TXT_RESTAURANT                        345
#define DFA_TXT_RESTAURANT                        0x3660C8

#define DFI_BTN_RETAIL2                           346
#define DFA_BTN_RETAIL2                           0x366AC2

#define DFI_BTN_RETAIL                            347
#define DFA_BTN_RETAIL                            0x367BA8

#define DFI_TXT_RETAIL                            348
#define DFA_TXT_RETAIL                            0x36A752

#define DFI_BTN_VACATION2                         349
#define DFA_BTN_VACATION2                         0x36ABBA

#define DFI_BTN_VACATION                          350
#define DFA_BTN_VACATION                          0x36BC34

#define DFI_TXT_VACATION                          351
#define DFA_TXT_VACATION                          0x36E7DE

#define DFI_BTN_WELLBEING2                        352
#define DFA_BTN_WELLBEING2                        0x36EF9C

#define DFI_BTN_WELLBEING                         353
#define DFA_BTN_WELLBEING                         0x36FF38

#define DFI_TXT_WELLBEING                         354
#define DFA_TXT_WELLBEING                         0x372AE2

#define DFI_ARMED_STAY                            355
#define DFA_ARMED_STAY                            0x3733F2

#define DFI_ARMED_AWAY                            356
#define DFA_ARMED_AWAY                            0x3791B2

#define DFI_DUMMY_6                               357
#define DFA_DUMMY_6                               0x37EF72

#define DFI_DUMMY_5                               358
#define DFA_DUMMY_5                               0x37F194

#define DFI_DUMMY_7                               359
#define DFA_DUMMY_7                               0x37F3B6

#define DFI_DUMMY_8                               360
#define DFA_DUMMY_8                               0x37F5D8

#define DFI_CLOCK_STRIKETHROUGH                   361
#define DFA_CLOCK_STRIKETHROUGH                   0x37F7FA

#define DFI_CLOCK                                 362
#define DFA_CLOCK                                 0x383E5C

#define DFI_BTN_GRAY_NA                           363
#define DFA_BTN_GRAY_NA                           0x3884BE

#define DFI_MOD_INPUT_WINDOW                      364
#define DFA_MOD_INPUT_WINDOW                      0x389E59

#define DFI_MOD_INPUT_WINDOW_DOUBLE_HEIGHT        365
#define DFA_MOD_INPUT_WINDOW_DOUBLE_HEIGHT        0x38C187

#define DFI_MOD_INPUT_WINDOW_SHORT                366
#define DFA_MOD_INPUT_WINDOW_SHORT                0x38FF6D

#define DFI_MOD_INPUT_WINDOW_LIST                 367
#define DFA_MOD_INPUT_WINDOW_LIST                 0x39172E

#define DFI_BTN_ACCESS_CODES_STATIC               368
#define DFA_BTN_ACCESS_CODES_STATIC               0x393228

#define DFI_BTN_ACCESS_CODES_TOUCH                369
#define DFA_BTN_ACCESS_CODES_TOUCH                0x39502E

#define DFI_BTN_AUDIO_VERIFICATION_STATIC         370
#define DFA_BTN_AUDIO_VERIFICATION_STATIC         0x396E34

#define DFI_BTN_AUDIO_VERIFICATION_TOUCH          371
#define DFA_BTN_AUDIO_VERIFICATION_TOUCH          0x398C3A

#define DFI_BTN_COMMUNICATION_MODES_STATIC        372
#define DFA_BTN_COMMUNICATION_MODES_STATIC        0x39AA40

#define DFI_BTN_COMMUNICATION_MODES_TOUCH         373
#define DFA_BTN_COMMUNICATION_MODES_TOUCH         0x39C846

#define DFI_BTN_LIGHT_CONTROL_STATIC              374
#define DFA_BTN_LIGHT_CONTROL_STATIC              0x39E64C

#define DFI_BTN_LIGHT_CONTROL_TOUCH               375
#define DFA_BTN_LIGHT_CONTROL_TOUCH               0x3A0452

#define DFI_BTN_PHONE_NUMBERS_STATIC              376
#define DFA_BTN_PHONE_NUMBERS_STATIC              0x3A2258

#define DFI_BTN_PHONE_NUMBERS_TOUCH               377
#define DFA_BTN_PHONE_NUMBERS_TOUCH               0x3A405E

#define DFI_BTN_PHONE_OPTIONS_STATIC              378
#define DFA_BTN_PHONE_OPTIONS_STATIC              0x3A5E64

#define DFI_BTN_PHONE_OPTIONS_TOUCH               379
#define DFA_BTN_PHONE_OPTIONS_TOUCH               0x3A7C6A

#define DFI_BTN_REPORT_OPTIONS_STATIC             380
#define DFA_BTN_REPORT_OPTIONS_STATIC             0x3A9A70

#define DFI_BTN_REPORT_OPTIONS_TOUCH              381
#define DFA_BTN_REPORT_OPTIONS_TOUCH              0x3AB876

#define DFI_BTN_REPORTING_STATIC                  382
#define DFA_BTN_REPORTING_STATIC                  0x3AD67C

#define DFI_BTN_REPORTING_TOUCH                   383
#define DFA_BTN_REPORTING_TOUCH                   0x3AF482

#define DFI_BTN_SECURITY_STATIC                   384
#define DFA_BTN_SECURITY_STATIC                   0x3B1288

#define DFI_BTN_SECURITY_TOUCH                    385
#define DFA_BTN_SECURITY_TOUCH                    0x3B308E

#define DFI_BTN_SENSORS_STATIC                    386
#define DFA_BTN_SENSORS_STATIC                    0x3B4E94

#define DFI_BTN_SENSORS_TOUCH                     387
#define DFA_BTN_SENSORS_TOUCH                     0x3B6C9A

#define DFI_BTN_SIREN_OPTIONS_STATIC              388
#define DFA_BTN_SIREN_OPTIONS_STATIC              0x3B8AA0

#define DFI_BTN_SIREN_OPTIONS_TOUCH               389
#define DFA_BTN_SIREN_OPTIONS_TOUCH               0x3BA8A6

#define DFI_BTN_SYSTEM_OPTIONS_STATIC             390
#define DFA_BTN_SYSTEM_OPTIONS_STATIC             0x3BC6AC

#define DFI_BTN_SYSTEM_OPTIONS_TOUCH              391
#define DFA_BTN_SYSTEM_OPTIONS_TOUCH              0x3BE4B2

#define DFI_BTN_SYSTEM_TESTS_STATIC               392
#define DFA_BTN_SYSTEM_TESTS_STATIC               0x3C02B8

#define DFI_BTN_SYSTEM_TESTS_TOUCH                393
#define DFA_BTN_SYSTEM_TESTS_TOUCH                0x3C20BE

#define DFI_BTN_TIMERS_STATIC                     394
#define DFA_BTN_TIMERS_STATIC                     0x3C3EC4

#define DFI_BTN_TIMERS_TOUCH                      395
#define DFA_BTN_TIMERS_TOUCH                      0x3C5CCA

#define DFI_BTN_TOUCHPAD_OPTIONS_STATIC           396
#define DFA_BTN_TOUCHPAD_OPTIONS_STATIC           0x3C7AD0

#define DFI_BTN_TOUCHPAD_OPTIONS_TOUCH            397
#define DFA_BTN_TOUCHPAD_OPTIONS_TOUCH            0x3C98D6

#define DFI_CHECKBOX_UNCHECKED                    398
#define DFA_CHECKBOX_UNCHECKED                    0x3CB6DC

#define DFI_CHECKBOX_CHECKED                      399
#define DFA_CHECKBOX_CHECKED                      0x3CBE35

#define DFI_RADIO_BUTTON_NOT_SELECTED             400
#define DFA_RADIO_BUTTON_NOT_SELECTED             0x3CC58E

#define DFI_RADIO_BUTTON_SELECTED                 401
#define DFA_RADIO_BUTTON_SELECTED                 0x3CCCE7

#define DFI_KEYPAD_A_STATIC                       402
#define DFA_KEYPAD_A_STATIC                       0x3CD440

#define DFI_KEYPAD_A_TOUCH                        403
#define DFA_KEYPAD_A_TOUCH                        0x3CEC01

#define DFI_KEYPAD_B_STATIC                       404
#define DFA_KEYPAD_B_STATIC                       0x3D03C2

#define DFI_KEYPAD_B_TOUCH                        405
#define DFA_KEYPAD_B_TOUCH                        0x3D1B83

#define DFI_KEYPAD_C_STATIC                       406
#define DFA_KEYPAD_C_STATIC                       0x3D3344

#define DFI_KEYPAD_C_TOUCH                        407
#define DFA_KEYPAD_C_TOUCH                        0x3D4B05

#define DFI_KEYPAD_D_STATIC                       408
#define DFA_KEYPAD_D_STATIC                       0x3D62C6

#define DFI_KEYPAD_D_TOUCH                        409
#define DFA_KEYPAD_D_TOUCH                        0x3D7A87

#define DFI_KEYPAD_E_STATIC                       410
#define DFA_KEYPAD_E_STATIC                       0x3D9248

#define DFI_KEYPAD_E_TOUCH                        411
#define DFA_KEYPAD_E_TOUCH                        0x3DAA09

#define DFI_KEYPAD_F_STATIC                       412
#define DFA_KEYPAD_F_STATIC                       0x3DC1CA

#define DFI_KEYPAD_F_TOUCH                        413
#define DFA_KEYPAD_F_TOUCH                        0x3DD98B

#define DFI_BTN_BACKSPACE_STATIC                  414
#define DFA_BTN_BACKSPACE_STATIC                  0x3DF14C

#define DFI_BTN_BACKSPACE_TOUCH                   415
#define DFA_BTN_BACKSPACE_TOUCH                   0x3E074E

#define DFI_BTN_DELETE_STATIC                     416
#define DFA_BTN_DELETE_STATIC                     0x3E1D50

#define DFI_BTN_DELETE_TOUCH                      417
#define DFA_BTN_DELETE_TOUCH                      0x3E2EEA

#define DFI_BTN_EDIT_STATIC                       418
#define DFA_BTN_EDIT_STATIC                       0x3E4084

#define DFI_BTN_EDIT_TOUCH                        419
#define DFA_BTN_EDIT_TOUCH                        0x3E521E

#define DFI_BTN_KEYPAD_STATIC                     420
#define DFA_BTN_KEYPAD_STATIC                     0x3E63B8

#define DFI_BTN_KEYPAD_TOUCH                      421
#define DFA_BTN_KEYPAD_TOUCH                      0x3E79BA

#define DFI_BTN_LIST_STATIC                       422
#define DFA_BTN_LIST_STATIC                       0x3E8FBC

#define DFI_BTN_LIST_TOUCH                        423
#define DFA_BTN_LIST_TOUCH                        0x3EA5BE

#define DFI_BTN_OK_MOD_STATIC                     424
#define DFA_BTN_OK_MOD_STATIC                     0x3EBBC0

#define DFI_BTN_OK_MOD_TOUCH                      425
#define DFA_BTN_OK_MOD_TOUCH                      0x3EE58A

#define DFI_BTN_DELETE_SENSOR_STATIC              426
#define DFA_BTN_DELETE_SENSOR_STATIC              0x3F0F54

#define DFI_BTN_DELETE_SENSOR_TOUCH               427
#define DFA_BTN_DELETE_SENSOR_TOUCH               0x3F2D5A

#define DFI_BTN_EDIT_SENSOR_STATIC                428
#define DFA_BTN_EDIT_SENSOR_STATIC                0x3F4B60

#define DFI_BTN_EDIT_SENSOR_TOUCH                 429
#define DFA_BTN_EDIT_SENSOR_TOUCH                 0x3F6966

#define DFI_BTN_INTERACTIVE_SERVICES_STATIC       430
#define DFA_BTN_INTERACTIVE_SERVICES_STATIC       0x3F876C

#define DFI_BTN_INTERACTIVE_SERVICES_TOUCH        431
#define DFA_BTN_INTERACTIVE_SERVICES_TOUCH        0x3FA572

#define DFI_BTN_LEARN_SENSOR_STATIC               432
#define DFA_BTN_LEARN_SENSOR_STATIC               0x3FC378

#define DFI_BTN_LEARN_SENSOR_TOUCH                433
#define DFA_BTN_LEARN_SENSOR_TOUCH                0x3FE17E

#define DFI_TXT_EDIT_SENSOR_HEADER                434
#define DFA_TXT_EDIT_SENSOR_HEADER                0x3FFF84

#define DFI_MOD_INPUT_WINDOW_SHORTER              435
#define DFA_MOD_INPUT_WINDOW_SHORTER              0x4015B0

#define DFI_TXT_PLEASEWAIT                        436
#define DFA_TXT_PLEASEWAIT                        0x40216E

#define DFI_IMG_BULLET_0                          437
#define DFA_IMG_BULLET_0                          0x4051AA

#define DFI_IMG_BULLET_1                          438
#define DFA_IMG_BULLET_1                          0x4057DD

#define DFI_IMG_BULLET_2                          439
#define DFA_IMG_BULLET_2                          0x4059AE

#define DFI_IMG_BULLET_3                          440
#define DFA_IMG_BULLET_3                          0x405C7E

#define DFI_TXT_EDIT_SENSOR_NAME_HEADER           441
#define DFA_TXT_EDIT_SENSOR_NAME_HEADER           0x4060B3

#define DFI_BTN_COMM_TEST_STATIC                  442
#define DFA_BTN_COMM_TEST_STATIC                  0x406C8F

#define DFI_BTN_COMM_TEST_TOUCH                   443
#define DFA_BTN_COMM_TEST_TOUCH                   0x408A95

#define DFI_BTN_SENSOR_TEST_STATIC                444
#define DFA_BTN_SENSOR_TEST_STATIC                0x40A89B

#define DFI_BTN_SENSOR_TEST_TOUCH                 445
#define DFA_BTN_SENSOR_TEST_TOUCH                 0x40C6A1

#define DFI_BTN_DWNLOAD_STATIC                    446
#define DFA_BTN_DWNLOAD_STATIC                    0x40E4A7

#define DFI_BTN_DWNLOAD_TOUCH                     447
#define DFA_BTN_DWNLOAD_TOUCH                     0x4102AD

#define DFI_TXT_SENSOR_TEST_HEADER                448
#define DFA_TXT_SENSOR_TEST_HEADER                0x4120B3

#define DFI_CAMERA                                449
#define DFA_CAMERA                                0x4165E9

#define DFI_DUMMY_3                               450
#define DFA_DUMMY_3                               0x416E29

#define DFI_TXT_IMAGES                            451
#define DFA_TXT_IMAGES                            0x41704B

#define DFI_TXT_DIRECTBYPASS                      452
#define DFA_TXT_DIRECTBYPASS                      0x4175CD

#define DFI_BTN_SELECT_STATIC                     453
#define DFA_BTN_SELECT_STATIC                     0x41862C

#define DFI_BTN_SELECT_TOUCH                      454
#define DFA_BTN_SELECT_TOUCH                      0x41B2D2

#define DFI_TXT_PROGRAMMING                       455
#define DFA_TXT_PROGRAMMING                       0x41DF78

#define DFI_TXT_MOTIONCHIME                       456
#define DFA_TXT_MOTIONCHIME                       0x41EFD7

#define DFI_TXT_DIRECT_BYPASS_HEADER              457
#define DFA_TXT_DIRECT_BYPASS_HEADER              0x41FD6C

#define DFI_BTN_BYPASSED_SENSOR_STATIC            458
#define DFA_BTN_BYPASSED_SENSOR_STATIC            0x4233FC

#define DFI_BTN_NOTBYPASSED_SENSOR_STATIC         459
#define DFA_BTN_NOTBYPASSED_SENSOR_STATIC         0x424596

#define DFI_BTN_QUICKEXIT_STATIC                  460
#define DFA_BTN_QUICKEXIT_STATIC                  0x425730

#define DFI_BTN_QUICKEXIT_TOUCH                   461
#define DFA_BTN_QUICKEXIT_TOUCH                   0x4280FA

#define DFI_AM_TIME_BOX                           462
#define DFA_AM_TIME_BOX                           0x42AAC4

#define DFI_PM_TIME_BOX                           463
#define DFA_PM_TIME_BOX                           0x42B682

#define DFI_MOD_INPUT_WINDOW_WIDE                 464
#define DFA_MOD_INPUT_WINDOW_WIDE                 0x42C240

#define DFI_BTN_123_STATIC                        465
#define DFA_BTN_123_STATIC                        0x42F9F0

#define DFI_BTN_123_TOUCH                         466
#define DFA_BTN_123_TOUCH                         0x430B8A

#define DFI_BTN_ABC_STATIC                        467
#define DFA_BTN_ABC_STATIC                        0x431D24

#define DFI_BTN_ABC_TOUCH                         468
#define DFA_BTN_ABC_TOUCH                         0x432EBE

#define DFI_BTN_BLANK_CIRCLE_STATIC               469
#define DFA_BTN_BLANK_CIRCLE_STATIC               0x434058

#define DFI_BTN_BLANK_CIRCLE_TOUCH                470
#define DFA_BTN_BLANK_CIRCLE_TOUCH                0x43531E

#define DFI_BTN_BLANK_SMALL_STATIC                471
#define DFA_BTN_BLANK_SMALL_STATIC                0x4365E4

#define DFI_BTN_BLANK_SMALL_TOUCH                 472
#define DFA_BTN_BLANK_SMALL_TOUCH                 0x43777E

#define DFI_BTN_BLANK_MEDIUM_STATIC               473
#define DFA_BTN_BLANK_MEDIUM_STATIC               0x438918

#define DFI_BTN_BLANK_MEDIUM_TOUCH                474
#define DFA_BTN_BLANK_MEDIUM_TOUCH                0x43A71E

#define DFI_BTN_BLANK_LARGE_STATIC                475
#define DFA_BTN_BLANK_LARGE_STATIC                0x43C524

#define DFI_BTN_BLANK_LARGE_TOUCH                 476
#define DFA_BTN_BLANK_LARGE_TOUCH                 0x43F1CA

#define DFI_BTN_SET_DATE_TIME_STATIC              477
#define DFA_BTN_SET_DATE_TIME_STATIC              0x441E70

#define DFI_BTN_SET_DATE_TIME_TOUCH               478
#define DFA_BTN_SET_DATE_TIME_TOUCH               0x443C76

#define DFI_BTN_PHOTO_ZOOM_IN                     479
#define DFA_BTN_PHOTO_ZOOM_IN                     0x445A7C

#define DFI_KEYPAD_PAUSE_STATIC                   480
#define DFA_KEYPAD_PAUSE_STATIC                   0x44F082

#define DFI_KEYPAD_PAUSE_TOUCH                    481
#define DFA_KEYPAD_PAUSE_TOUCH                    0x450843

#define DFI_BTN_CANCEL_MODIFICATION_STATIC        482
#define DFA_BTN_CANCEL_MODIFICATION_STATIC        0x452004

#define DFI_BTN_CANCEL_MODIFICATION_TOUCH         483
#define DFA_BTN_CANCEL_MODIFICATION_TOUCH         0x453F2A

#define DFI_BTN_SAVE_MODIFICATION_STATIC          484
#define DFA_BTN_SAVE_MODIFICATION_STATIC          0x45552C

#define DFI_BTN_SAVE_MODIFICATION_TOUCH           485
#define DFA_BTN_SAVE_MODIFICATION_TOUCH           0x4573F2

#define DFI_BTN_ENTER_MODIFICATION_STATIC         486
#define DFA_BTN_ENTER_MODIFICATION_STATIC         0x4589F4

#define DFI_BTN_ENTER_MODIFICATION_TOUCH          487
#define DFA_BTN_ENTER_MODIFICATION_TOUCH          0x459B8E

#define DFI_BTN_ENTER_STATIC                      488
#define DFA_BTN_ENTER_STATIC                      0x45AD28

#define DFI_BTN_ENTER_TOUCH                       489
#define DFA_BTN_ENTER_TOUCH                       0x45D9CE

#define DFI_ARMED_AWAY_MOTIONS                    490
#define DFA_ARMED_AWAY_MOTIONS                    0x460674

#define DFI_MOD_SCREEN_INPUT_WINDOW               491
#define DFA_MOD_SCREEN_INPUT_WINDOW               0x466434

#define DFI_MOD_SCREEN_INPUT_WINDOW_DOUBLE_HEIGHT 492
#define DFA_MOD_SCREEN_INPUT_WINDOW_DOUBLE_HEIGHT 0x468D3E

#define DFI_CAUTION_TRIANGLE                      493
#define DFA_CAUTION_TRIANGLE                      0x46D574

#define DFI_BTN_COG_CAUTION_TRIANGLE              494
#define DFA_BTN_COG_CAUTION_TRIANGLE              0x46D9E8

#define DFI_BTN_HEAR_CAUTION_TRIANGLE             495
#define DFA_BTN_HEAR_CAUTION_TRIANGLE             0x46E2F4

#define DFI_TXT_SYSTEM_TESTS                      496
#define DFA_TXT_SYSTEM_TESTS                      0x46FC8F

#define DFI_TXT_SETDATETIME                       497
#define DFA_TXT_SETDATETIME                       0x470CEE

#define DFI_KEYPADB_0_STATIC                      498
#define DFA_KEYPADB_0_STATIC                      0x4724E8

#define DFI_KEYPADB_0_TOUCH                       499
#define DFA_KEYPADB_0_TOUCH                       0x4734C0

#define DFI_KEYPADB_1_STATIC                      500
#define DFA_KEYPADB_1_STATIC                      0x474498

#define DFI_KEYPADB_1_TOUCH                       501
#define DFA_KEYPADB_1_TOUCH                       0x475470

#define DFI_KEYPADB_2_STATIC                      502
#define DFA_KEYPADB_2_STATIC                      0x476448

#define DFI_KEYPADB_2_TOUCH                       503
#define DFA_KEYPADB_2_TOUCH                       0x477420

#define DFI_KEYPADB_3_STATIC                      504
#define DFA_KEYPADB_3_STATIC                      0x4783F8

#define DFI_KEYPADB_3_TOUCH                       505
#define DFA_KEYPADB_3_TOUCH                       0x4793D0

#define DFI_KEYPADB_4_STATIC                      506
#define DFA_KEYPADB_4_STATIC                      0x47A3A8

#define DFI_KEYPADB_4_TOUCH                       507
#define DFA_KEYPADB_4_TOUCH                       0x47B380

#define DFI_KEYPADB_5_STATIC                      508
#define DFA_KEYPADB_5_STATIC                      0x47C358

#define DFI_KEYPADB_5_TOUCH                       509
#define DFA_KEYPADB_5_TOUCH                       0x47D330

#define DFI_KEYPADB_6_STATIC                      510
#define DFA_KEYPADB_6_STATIC                      0x47E308

#define DFI_KEYPADB_6_TOUCH                       511
#define DFA_KEYPADB_6_TOUCH                       0x47F2E0

#define DFI_KEYPADB_7_STATIC                      512
#define DFA_KEYPADB_7_STATIC                      0x4802B8

#define DFI_KEYPADB_7_TOUCH                       513
#define DFA_KEYPADB_7_TOUCH                       0x481290

#define DFI_KEYPADB_8_STATIC                      514
#define DFA_KEYPADB_8_STATIC                      0x482268

#define DFI_KEYPADB_8_TOUCH                       515
#define DFA_KEYPADB_8_TOUCH                       0x483240

#define DFI_KEYPADB_9_STATIC                      516
#define DFA_KEYPADB_9_STATIC                      0x484218

#define DFI_KEYPADB_9_TOUCH                       517
#define DFA_KEYPADB_9_TOUCH                       0x4851F0

#define DFI_KEYPADB_POUND_STATIC                  518
#define DFA_KEYPADB_POUND_STATIC                  0x4861C8

#define DFI_KEYPADB_POUND_TOUCH                   519
#define DFA_KEYPADB_POUND_TOUCH                   0x4871A0

#define DFI_KEYPADB_STAR_STATIC                   520
#define DFA_KEYPADB_STAR_STATIC                   0x488178

#define DFI_KEYPADB_STAR_TOUCH                    521
#define DFA_KEYPADB_STAR_TOUCH                    0x489150

#define DFI_BTN_CLOSE_JPEG_STATIC                 522
#define DFA_BTN_CLOSE_JPEG_STATIC                 0x48A128

#define DFI_HEADER_BEVEL                          523
#define DFA_HEADER_BEVEL                          0x48BA7E

#define DFI_TOTAL                                 524
#define DFA_TOTAL                                 0x48E784

#endif/* DFFORMAT_H */
