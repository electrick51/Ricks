/*
 * File:   UIthread.c
 * Author: Rschneew
 *
 * Created on April 9, 2018, 6:45 PM
 */


#include "xc.h"

#include "dataflash.h"
#include "threads.h"
#include "queue.h"
#include "rtcc.h"
#include "uimanager.h"
#include "sys_variables.h"
#include "dfformat.h"
//#include "eventscreen.h"
//#include "dealercontactscreen.h"
#include "sensor.h"
//#include "mainscreen.h"
//#include "voice.h"
#include "sysFunctions.h"
//#include "graphics\graphics_app_layer.h"
//#include "sensorscreen.h"
//#include "keypadscreen.h"
//#include "graphics_image.h"
#include "resources.h"
//#include "blank_screen.h"
#include "ULPKTDEF.H"
#include "UImanager.h"    
#include "leds.h"
#include "debugMessages.h"


static void UI_thread(void);
static const char *p_token_to_text[];
extern struct Message_Data event_msg;

#define UI_QSIZE 10
static struct message_Q UI_message_Q;				//container information... number of messages in que.
static struct Message_Data UI_Q[UI_QSIZE]; //declare message storage array
	
//extern struct PROTEST_INFO protest_info;           //contains protest data sent by the panel 
extern struct SYS_STATUS sys_UI;

extern struct Touch_Screen_Data touchscreen_data;
   
extern uint8_t scratch_buffer[];    
#if SCRATCH_BUFFER_SZ < STREAM_BLOCK_SZ            //cause a compiler error if scratch buffer is too small
    error scratch buffer too small
#endif 
extern struct SENSORQUE sensorque[]; 

/**
 *
 * create the UI manager thread
 *
 */
void UI_thread_init(void)
{
   	static THREAD UI_thread_struct;
   	static uint8_t UI_thread_stack[1000];    // TBD determine needed size 
    
    //UI thread initialize();
	thread_create(UI_thread, &UI_thread_struct, UI_thread_stack, sizeof(UI_thread_stack));
}

/**
 *
 * place a message in the UI mangers inbox
 *
 */
void UI_message_put(struct Message_Data *new_msg)
{
//	message_Q_put(&UI_Q,new_msg);
	message_Q_put(&UI_message_Q, new_msg);
}

/**
 *
 * get a message from the UI mangers inbox
 *
 */
struct Message_Data *UI_message_get(void)
{
    return message_Q_get(&UI_message_Q);
}

/**
 *
 * Initialize the UI manager message q
 *
 */
void UI_message_init(void)
{
	message_Q_initialize(&UI_message_Q,UI_Q, UI_QSIZE);    	//initialize message q.    
}



//*********************************************************UI thread **********************************************

/**
 *
 * UI manager thread - never returns
 *
 */
static void UI_thread(void)
{
    
    LED_Enable (LED_D3);
    LED_Enable (LED_D4);
    LED_Enable (LED_D5);
    LED_Enable (LED_D8);
    static int LoopTimer2 = 0;
    enum Keys keyValue;
    
   
   struct Message_Data *pevent_msg;

   for(;;)								//threads should not return
   {

        LoopTimer2++;
       if(LoopTimer2 > 20000)
       {
           
           
           LoopTimer2 = 0;
       }
        LED_On(LED_D8);
        pevent_msg = UI_message_get();
        
//        sendText("UIMessageGet", 1, 0);
//    thread_wait(10);
//    
//enum Keys
//{
//        ZeroKey=0,
//    OneKey=1,
//    TwoKey=2,
//    ThreeKey=3,
//    FourKey=4,
//    FiveKey=5,
//    SixKey=6,
//    SevenKey=7,
//    EightKey=8,
//    NineKey=9,
//    StatusKey,
//    BypassKey,
//    
//    EmergencyKey,
//    
//    DisarmKey,
//    ArmDwKey,
//    ArmMotsKey,
//    ArmAllKey,
//    LightsOnKey,
//    LightsOffKey
//    
//};
        if(pevent_msg)
	{
            sendText("Got UI Message\n\r", 1, 0);
            thread_wait(10);
                        
                switch(pevent_msg->tag)
                {
                        case msg_keypress:
                            event_msg.tag = msg_keypress;
                            event_msg.message_info.uplink_pkt.ulink_pkt_type = type_keypress;
                            keyValue = event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode;
                            
                            switch(keyValue)
                            {
                                case uiZeroKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = ZeroKey;
                                    break;
                                case uiOneKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = OneKey;
                                    break;
                                case uiTwoKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = TwoKey;
                                    break;
                                case uiThreeKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = ThreeKey;
                                    break;
                                case uiFourKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = FourKey;
                                    break;
                                case uiFiveKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = FiveKey;
                                    break;
                                case uiSixKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = SixKey;
                                    break;
                                case uiSevenKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = SevenKey;
                                    break;
                                case uiEightKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = EightKey;
                                    break;
                                case uiNineKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = NineKey;
                                    break;
                                case uiStatusKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = StatusKey;
                                    break;
                                case uiBypassKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = Bypass;
                                    break;
                                case uiEmergencyKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = EmergencyKey;
                                    break;
                                case uiDisarmKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = DisarmKey;
                                    break;
                                case uiArmDwKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = ArmDwKey;
                                    break;
                                case uiArmMotsKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = ArmMotsKey;
                                    break;
                                case uiArmAllKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = ArmAllKey;
                                    break;
                                case uiLightsOnKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = _LightsOnKey;
                                    break;
                                case uiLightsOffKey:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = _LightsOffKey;
                                    break;
                                case uiEnrollMeRequest:
                                    event_msg.message_info.uplink_pkt.uplink_pkts.key_value.keycode = EnrollMeRequest;
                                    break;
                                     
                                    
                            }
                            pnlComm_put(&event_msg);        
                            break;
                            

                }// switch	
	}// event_msg
		LED_Off(LED_D8);
		context_switch();				            //no message received so sleep for 100mS second - allow other threads to run
	}         
}