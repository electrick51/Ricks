/**
 * Filename: queue.h
 *  Version: 1.0.0
 *
 *  define system message passing structures
 *
 */
#ifndef QUEUE_H
#define QUEUE_H

struct STREAM_DATA;
struct Message_Data;        //forward declare the message structure

#include "exactint.h"
#include "uimanager.h"
#include "sys_variables.h"
#include "keypress.h"
#include "display.h"
#include "ulpktdef.h"
#include "dlpktdef.h"
#include "Timers.h"


#define NO_GRAPHICS_METHOD_STRUCT


#include "sensor.h"

//note enums with assigned values must not be changed...ex) msg_panel_arm_state must==0 as this is the value that is passed in
//bits 31-36 of the system ping message.
enum Message_Tags 
{  
    msg_panel_arming_status=0,
    msg_flush_Q,
    msg_keypress, 
    msg_voice_phrase_data,
    msg_say_audio_token, 	
    msg_set_volume,  
    msg_learn_tp,
    msg_display_method, 
    msg_display_dealer_contact,
    msg_display_main,
    msg_display_status_settings, 
    msg_display_calibration,
    msg_display_disarm, 
    msg_rf_rx, 
    msg_rf_tx,
    msg_protest_info,                    //when received => the panel is protesting
    msg_systatus,
    msg_stream_data,
    msg_voice_report,
    msg_voice_purge,
    msg_valid_user_code,
    msg_validate_user_code,
    msg_debug_message,
    msg_none = 0xff                     //always make this the last enumeration         
};

enum Keys
{
    uiZeroKey=0,
    uiOneKey=1,
    uiTwoKey=2,
    uiThreeKey=3,
    uiFourKey=4,
    uiFiveKey=5,
    uiSixKey=6,
    uiSevenKey=7,
    uiEightKey=8,
    uiNineKey=9,
    uiStatusKey,
    uiBypassKey,
    
    uiEmergencyKey,
    
    uiDisarmKey,
    uiArmDwKey,
    uiArmMotsKey,
    uiArmAllKey,
    uiLightsOnKey,
    uiLightsOffKey,
    uiEnrollMeRequest
};

enum Stream_Status
{
    stream_start=0,
    stream_inprocess, 
    stream_done,
};       
      
enum Alarm_State
{
    no_alarm, 
    burg_alarm, 
    fire_alarm, 
    emergency_alarm
};

//note enum must match dlprotocol device type. see dlproto message 71h
#define EQUIP_ENABLE 0xff
#define CONTROL_ALL 0xfe
enum AUTOMATION_UNIT_TYPE
{
    type_light=0,
    type_lock=1
}; 

#define MAX_DATA_LEN 10  //KGS TBD remove???

struct message_Q
{
	uint8_t in,out;			//message q indexesi
	//SEMAPHORE semaphore;
	struct Message_Data
	{
		enum Message_Tags tag;
		union Message_Info
	    {
			struct ArmingState panel_arming_state;
	    	
			// Data received from RF message
			struct RF_PACKET
			{
				struct RF_Message_Pkt rf_message;
				
			}rf_packet_data;
			
			struct SYS_STATUS sys_status;
			
            struct DebugMessage
            {
                
                uint8_t size;
                uint8_t type;
                uint8_t mod[4];
                uint8_t textBuffer[30];
            }debugMsg;
            
			// Timer message 
			struct Timer_Message
			{
				uint16_t duration;				// Time in seconds
				uint16_t sender;				// Don't think this is needed - TBD remove
				struct SecTimer_tag *myTimer;	// SecTimer declared in sending module
			}timer_message;	
/*
 Including the graphics method structure here.
*/
#ifndef NO_GRAPHICS_METHOD_STRUCT            
            struct GRAPHICS_METHOD graphics_method;
#endif				
			struct KEYPRESS_DATA
			{
				enum Keypress_Values  press;
				uint8_t press_tm;          //ammount of time in seconds that key has been held (not used as a debounce mechanism)
			    uint8_t value;             //slider value
			} keypress_data;
			
			struct PHRASE_DATA
			{
				uint16_t phraseN;
				uint8_t generic;                //content depends on phrase number   	
			    uint8_t generic2;               //content depends on phrase number
			} voice_phrase_data;
			
		    struct Say_Audio_Token
			{
			    uint8_t token;
			}say_audio_token;
			
			struct SET_VOLUME
			{
				uint8_t level;	
			}set_volume;	
#if 0			
			struct DISPLAY_DATA_OBSOLETE
			{
//				enum Display_Screen display_number;
				//uint8_t x_coordinate;
				//uint8_t y_coordinate;
			    uint8_t txt_data[MAX_DATA_LEN];					//tbd might need to be pointer, null terminated. 
			}display_data_OBSOLETE;
#endif			
			struct NoData
			{		                                                   //no data... All data is implicit for these messages
			}display_dealer_contact,display_main;
			
			struct SET_TMxx
			{
			    /* date and time components in packed format -- compressed to send over 319.5 channel*/
			    uint16_t tm_sec:6; 
			    uint16_t tm_min:6;
				uint16_t tm_hour:5;
				//uint16_t tm_wday:3;
				//uint16_t tm_mday:5;
				//uint16_t tm_mon:4;
				//uint16_t tm_year:5;              //good until 2009 + 32
			}set_tm;
		    struct SET_DATExx
			{
			  /* date and time components in packed format -- compressed to send over 319.5 channel*/
			   //uint16_t tm_sec:6; 
			   //uint16_t tm_min:6;
				//uint16_t tm_hour:5;
			    uint16_t tm_wday:3;
				uint16_t tm_mday:5;
				uint16_t tm_mon:4;
				uint16_t tm_year:6;              //good until 2009 + 64
			}set_date;
	        struct SYS_STATUS system_status;
         
            struct STREAM_DATA
            {
                enum Stream_Id stream_id;
                uint8_t *pData;                        //pointer to stream data
                uint8_t block_number;
                enum Stream_Status status;
            }stream_data;   	      

	        struct Uplink_Pkts uplink_pkt;


	        struct PROTEST_INFO
            {
                union PROTEST_SOURCE
                { 
                    uint8_t zone_1:1;                  //zone 1
                    uint8_t zone_2:1;                  //zone 2
                    uint8_t zone_3:1;                  //zone 3
                    uint8_t zone_4:1;                  //zone 4
                    uint8_t zone_5:1;                  //zone 5
                    uint8_t zone_6:1;                  //zone 6
                    uint8_t zone_7:1;                  //zone 7
                    uint8_t zone_8:1;                  //zone 8
                    uint8_t zone_9:1;                  //zone 9
                    uint8_t zone_10:1;                 //zone 10
                    uint8_t zone_11:1;                 //zone 11
                    uint8_t zone_12:1;                 //zone 12
                    uint8_t zone_13:1;                 //zone 13
                    uint8_t zone_14:1;                 //zone 14
                    uint8_t zone_15:1;                 //zone 15
                    uint8_t zone_16:1;                 //zone 16
                    uint8_t zone_17:1;                 //zone 17
                    uint8_t zone_18:1;                 //zone 18
                    uint8_t zone_19:1;                 //zone 19
                    uint8_t zone_20:1;                 //zone 20  
                    uint8_t zone_21:1;                 //zone 21
                    uint8_t zone_22:1;                 //zone 22
                    uint8_t zone_23:1;                 //zone 23
                    uint8_t zone_24:1;                 //zone 24
                    uint8_t zone_25:1;                 //zone 25
                    uint8_t zone_26:1;                 //zone 26
                    uint8_t zone_27:1;                 //zone 27
                    uint8_t zone_28:1;                 //zone 28
                    uint8_t zone_29:1;                 //zone 29
                    uint8_t zone_30:1;                 //zone 30
                    uint8_t zone_31:1;                 //zone 31
                    uint8_t zone_32:1;                 //zone 32
                    uint8_t zone_33:1;                 //zone 33
                    uint8_t zone_34:1;                 //zone 34
                    uint8_t zone_35:1;                 //zone 35
                    uint8_t zone_36:1;                 //zone 36
                    uint8_t zone_37:1;                 //zone 37
                    uint8_t zone_38:1;                 //zone 38
                    uint8_t zone_39:1;                 //zone 39
                    uint8_t zone_40:1;                 //zone 40
                    uint8_t AcPowerFail:1;             
                    uint8_t SystemBatFail:1;
                    uint8_t ReportingFail:1;
                    uint8_t PhoneLineFail:1;
                    uint8_t IntSirenTrbl:1;
                    uint8_t ExtSirenTrbl:1;  
                    uint8_t  zones[NUMBER_OF_ZONES/8];  //zones 1 - 40
                    uint8_t   all_system;               //system status
                }protest_source;
            }protest_info;
            
            struct ZWAVEEQUIPMENT
            {
                uint8_t Z_index;                    //zwave number
                //enum AUTOMATION_UNIT_TYPE type;     //0-light 1 lock    
                uint8_t type;                       //KGS would prefer to use AUTOMATION enum , however the storage size must be 1 byte and the MPLAB
                                                    //compliler resolves enums to 2 bytes.
                uint8_t state;
                uint8_t name[16];                   //token based name or name[0] = sensor number
            }zwave_info;
            struct CODE_REQUEST
            {
                enum SCREEN_FOCUS requestor;
                enum SCREEN_FOCUS cancel_requestor;
                enum Keypress_Values keypress;
                enum USERLVL user_lvl;
                uint8_t screen_page;         
            }code_request;
            struct CODE_REPONSE
            {
                enum Keypress_Values keypress;
                enum USERLVL user_lvl;
                uint8_t screen_page;
            }code_response;    
                              
	    }message_info;
	//	Message_Types message_type;		
    }message_data;
	uint8_t Q_size;			
	struct Message_Data *pMessage_Q;			//pointer to beginning of message Q;
};

void message_Q_initialize(struct message_Q *, struct Message_Data *, uint16_t);
struct Message_Data *message_Q_get(struct message_Q *);
void message_Q_put(struct message_Q *msg_Q, struct Message_Data *new_msg);
uint8_t message_Q_status(struct message_Q *msg_Q);

#endif

