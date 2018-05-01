//
// Statusmanager - Primary function is to update touchscreen variables
//                 that are sent by the panel.
//C:\Users\Rick\Documents\SimonXTX\SimonXTXInterfaceBd.X\TouchscreenFiles\statusmanager.c
//#include "twttsdef.h"
#include "dataflash.h"
#include "threads.h"
#include "queue.h"
#include "rtcc.h"
#include "uimanager.h"
#include "sys_variables.h"
#include "dfformat.h"
#include "debugMessages.h"
#include "sensor.h"

#include "sysFunctions.h"

#include "resources.h"
//#include "blank_screen.h"
#include "statusmanager.h"    
#include "leds.h"



static void status_manager_thread(void);
static const char *p_token_to_text[];
static struct Message_Data debugMsg;

#define status_QSIZE 10
static struct message_Q status_message_Q; //container information... number of messages in que.
static struct Message_Data status_Q[status_QSIZE]; //declare message storage array

//extern struct PROTEST_INFO protest_info;           //contains protest data sent by the panel 
extern struct SYS_STATUS sys_status;

//extern struct Touch_Screen_Data touchscreen_data;
extern struct VDATA vData;

extern uint8_t scratch_buffer[];
#if SCRATCH_BUFFER_SZ < STREAM_BLOCK_SZ            //cause a compiler error if scratch buffer is too small
error scratch buffer too small
#endif 
extern struct SENSORQUE sensorque[];

/**
 *
 * create the status manager thread
 *
 */
void status_manager_thread_init(void) 
{
    static THREAD status_manager_thread_struct;
    static uint8_t status_manager_thread_stack[1000]; // TBD determine needed size 

    //status_manager thread initialize();
    thread_create(status_manager_thread, &status_manager_thread_struct, status_manager_thread_stack, sizeof (status_manager_thread_stack));
}

/**
 *
 * place a message in the status mangers inbox
 *
 */
void status_message_put(struct Message_Data *new_msg) 
{
    //	message_Q_put(&status_Q,new_msg);
    message_Q_put(&status_message_Q, new_msg);
}

/**
 *
 * get a message from the status mangers inbox
 *
 */
struct Message_Data *status_message_get(void) 
{
    return message_Q_get(&status_message_Q);
}

/**
 *
 * Initialize the status manager message q
 *
 */
void status_manager_message_init(void) 
{
    message_Q_initialize(&status_message_Q, status_Q, status_QSIZE); //initialize message q.    
}

///**
// *
// *
// *
// */
void clear_delay_info(void)
{
struct VDATA vData;
   
    vData.delay_tm = 0;
    vData.entry_delay=0;
    vData.exit_delay=0;         
}   
//// 
/////**
//// *
//// *  update timers - called once a second
//// *
//// */
void update_sys_timers(void)
{
    extern struct VDATA vData;

    vData.running_seconds++;
    //
    // update the entry/exit delay time
    //
    if( vData.delay_tm)
    {
        vData.delay_tm--;
    }
    else
    {
        clear_delay_info();
    }
    //
    //update password tmr
    //
    if(vData.password_tmr)
    {
        vData.password_tmr--;    
    }
    else
    {   
        vData.user_lvl = undefined_lvl;  
    }          
    //
    // update minute timer
    //
    if(vData.running_seconds >= 60)
    {
        struct PANELDATA panelData;
        struct TimePkt time;
        struct DatePkt date;
        
        //update the time clock once a minute
//        panelData.inputDataLen = 0;
//        getPanelData(tTime, &panelData);
//        time.tm_hour = panelData.data.time.tm_hour;
//        time.tm_min = panelData.data.time.tm_min;        
//        time.tm_sec = panelData.data.time.tm_sec;
//        date.tm_mday = panelData.data.time.tm_mday;
//        date.tm_mon = panelData.data.time.tm_mon;
//        date.tm_wday = panelData.data.time.tm_wday;
//        date.tm_year = panelData.data.time.tm_year;       
//        RTCC_set(&time,&date);
        
        vData.running_seconds = 0;
        //RTCC_get(&vData.rtc);               //read the real time clock once a minute =>moved into  void draw_tm1() function
        vData.minute_tmr++;
//        updateBacklightBrightnessStatus(eUpdateLcdLedBrightnessTmr);
        sendDisplayVersion(0);        
        if(vData.minute_tmr >= 60 )
        {
            vData.minute_tmr = 0;
            vData.hour_tmr++;
        }
//        motion_tmrs_update();
    }
}

/**
 *
 * status manager thread - never returns
 *
 */
static void status_manager_thread(void) 
{

    LED_Enable(LED_D3);
    LED_Enable(LED_D4);
    LED_Enable(LED_D5);
    //    LED_Enable (LED_D6);
    static int LoopTimer2 = 0;

    //   extern struct NVDATA nvData;
    //   extern struct VDATA vData;
    //
    //   static struct TimePkt time;
    //   static struct TstatTemperatureDisplayPktA tstatPktA;
    //   static struct TstatTemperatureDisplayPktB *tstatPktB;
    //   static union TwttsEvent event;
    //   static struct WeatherForecastPktA weatherPktA;
       static uint16_t sec_tic;

    struct Message_Data *pevent_msg;
    //   static uint8_t scratch
    //   struct Message_Data ui_update_message;

    //   InitSensorQue();  // Initialize the sensor que 
    //   set_recheck_present_flags();         //initialize sensor flags
    ///*   
    //      FOR TEST ONLY
    //      
    //   AddSensorToQue(4);
    //   
    //   AddSensorToQue(6);
    //   
    //   AddSensorToQue(12);
    //   
    //   AddSensorToQue(9);
    //   
    //   AddSensorToQue(7);
    //   
    //   AddSensorToQue(10);
    //   AddSensorToQue(8);
    //   
    //   AddSensorToQue(13);
    //   
    //   AddSensorToQue(11);
    //*/   
    //   sec_tic = timers_seconds_get();
    for (;;) //threads should not return
    {
        //        if(runTextToImgRoutine())  //should the routine run?
        //        {
        //            AddTextToImg((uint8_t*)&nvData.service_provider_msg, sizeof(nvData.service_provider_msg));   //process one row each pass through the for loop
        //        }
        LoopTimer2++;
        if (LoopTimer2 > 20000) 
        {
            

            LoopTimer2 = 0;
        }

        LED_On(LED_D7);
        pevent_msg = status_message_get();

//                sendText("statusMessageGet", 1, 0);
//            thread_wait(10);
        //    
        if (pevent_msg) 
        {
            //                        sendText("Got Status Mgr Message\n\r", 1, 0);
            //                        thread_wait(10);

            switch (pevent_msg->tag) 
            {
                case msg_stream_data:
//                    buffer_stream(&pevent_msg->message_info.stream_data, pevent_msg->message_info.stream_data.stream_id);
                    break;
                case msg_rf_rx:
//                    sendText("Got RF packet\n\r", 1, 0);
//                    thread_wait(20);
                    switch (pevent_msg->message_info.rf_packet_data.rf_message.type) 
                    {
                        buildDebugMessage(&debugMsg, "Got an RF message packet\n\r", 1, 0, "SM");
                        debugMsg.tag = msg_debug_message;
                        DebugMessage_message_put(&debugMsg);
                        context_switch();
               //             sendText("Sending One key\n\r", 1, 0);
                        thread_wait(30);
                        case ArmingStateCode:
                            pevent_msg->tag = msg_panel_arming_status;
                            pevent_msg->message_info.panel_arming_state = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.arming_state;
//                            sendText("Got Arming State packet\n\r", 1, 0);
                            thread_wait(20);
                            buildDebugMessage(&debugMsg, "Got Arming State packet\n\r", 1, 0, "SM");
                            debugMsg.tag = msg_debug_message;
                            DebugMessage_message_put(&debugMsg);
                            context_switch();
               //             sendText("Sending One key\n\r", 1, 0);
                            thread_wait(30);
                            switch (pevent_msg->message_info.panel_arming_state.arming_status) 
                            {
                                case in_exit_delay:
                                case in_exit_delay_directed:
                                    vData.delay_tm = pevent_msg->message_info.panel_arming_state.arm_data.exit_delay;
                                    vData.exit_delay = vData.delay_tm > 0 ? 1 : 0;
                                    vData.entry_delay = 0;
                                    break;
                                case in_entry_delay:
                                    if (vData.panel_status.quick_exit_on == 0) 
                                    {
                                        vData.delay_tm = pevent_msg->message_info.panel_arming_state.arm_data.exit_delay;
                                        vData.entry_delay = vData.delay_tm > 0 ? 1 : 0;
                                    }
                                    vData.exit_delay = 0;
                            }
                            UI_message_put(pevent_msg);
                            break;
                            //		   		        case SystemStatusPktCode2:
//                            //set_panel_status(0,&pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.system_status_pkt2.systemStatus2);
                            //		   			        break;		   		        
                            //case SynchPctrPktCode:   // gone again - gmk
                        case SystemStatusPktCode: //the following cases contain the same data
                            buildDebugMessage(&debugMsg, "Got SystemStatusPktCode packet\n\r", 1, 0, "SM");
                            debugMsg.tag = msg_debug_message;
                            DebugMessage_message_put(&debugMsg);
                            context_switch();
               //             sendText("Sending One key\n\r", 1, 0);
                            thread_wait(30);
                            break;
                        case SystemPingPktCode:
                            
                            buildDebugMessage(&debugMsg, "Got Ping packet\n\r", 1, 0, "SM");
                            debugMsg.tag = msg_debug_message;
                            DebugMessage_message_put(&debugMsg);
                            context_switch();
               //             sendText("Sending One key\n\r", 1, 0);
                            thread_wait(30);
                            
//                            buildDebugMessage(&debugMsg, pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.system_status_pkt.systemStatus, 2, 8, "SM");
//                            debugMsg.tag = msg_debug_message;
//                            DebugMessage_message_put(&debugMsg);
//                            context_switch();
//               //             sendText("Sending One key\n\r", 1, 0);
//                            thread_wait(30);
                            
                            set_panel_status(&pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.system_status_pkt.systemStatus); //,0);
                            break;
                        case TxtTokenDisplayPktCode:
//                            sendText("Got Display packet\n\r", 1, 0);
                            thread_wait(20);
                            pevent_msg->tag = msg_keypress;
                            pevent_msg->message_info.keypress_data.press =
                                    parse_txt_number(pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.txt_token_display_pkt);
                            if (pevent_msg->message_info.keypress_data.press != key_not) 
                            {
                                //pevent_msg->message_info.protest_info = protest_info;
                                UI_message_put(pevent_msg);
                            }
                            break;
                        case TimePktCode:
//                            sendText("Got Time packet\n\r", 1, 0);
                            thread_wait(20);
                            //copy received time and send it to rtcc after receiving date information
                            //					   	    time = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.time_pkt; 
                            break;
                        case DatePktCode:
//                            sendText("Got Date packet\n\r", 1, 0);
                            thread_wait(20);
                            //set the RTTC whenever date command is received
//                            RTCC_set(&time, &pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.date_pkt);
                            break;
                            //A zone update message has been received
                        case ZoneStatusPktCode:
//                            sendText("Got Zone status packet\n\r", 1, 0);
                            thread_wait(20);
#ifndef QC376_FIX                            
                            vData.CHNDAT[pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.zoneNumber].chnByte =
                                    pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.chn_data.chnByte;
#else
                            update_chn_status(
                                    pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.zoneNumber,
                                    pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.chn_data.chnByte
                                    );
#endif    
//                            set_door_window_status();
                            //
                            //check for asset management sensor?
                            //
                            if (pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.asset_detected) 
                            {
//                                AddSensorToQue(pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.zoneNumber);
                            }
                            //
                            // has motion been detected
                            //
//                            if (pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.chime_motion_detected) 
                            //{
////                                set_motion_tmr(pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.zoneNumber,
//                                        //                                                MOTION_MIN_START_TM
//                                        );
//                            }
//                            else if (CheckIfWindowOrDoorOrTouchpadorMotion(pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.zoneNumber) == 0) {
//                                set_property_recheck(request_recheck);
//                            }
                            //
                            // is chime enabled ? Note: asset beep ignores the chime setting
                            //    					       
#if 0 //KGS not needed for integrated screen as the panel performs the chime? 					        
                            if (nvData.chime ||
                                    ((get_panel_firmware_ver()) > SIMONXTVER13 &&
                                    pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.asset_detected)) {
                                pevent_msg->tag = msg_voice_phrase_data;
                                //get the chime type
                                tmp = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.chime;
                                //set the sensor number
                                pevent_msg->message_info.voice_phrase_data.generic =
                                        pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.zone_status_pkt.zoneNumber;
                                switch (tmp) {
                                        //say sensor name
                                    case 1:
                                        pevent_msg->message_info.voice_phrase_data.phraseN = T_SensorChime;
                                        //					                    voice_message_put(pevent_msg);
                                        break;
                                        //chime sound 1
                                    case 2:
                                        pevent_msg->message_info.voice_phrase_data.phraseN = T_SensorChime2;
                                        //					                    voice_message_put(pevent_msg);
                                        break;
                                        //chime sound 2
                                    case 3:
                                        pevent_msg->message_info.voice_phrase_data.phraseN = T_SensorChime3;
                                        //					                    voice_message_put(pevent_msg);
                                        break;
                                    case 4:
                                        vData.asset_beep = 1;
                                        break;
                                }
                            }
#endif					        
                            break;
                            //a new event has been received from the panel
                            //save it in the event buffer
                        case EvtPktACode:
//                            sendText("Got EvtPktACode packet\n\r", 1, 0);
                            thread_wait(20);
                            //copy 3 bytes (EvtPktACode)
                            //					        event.eventAll[0] = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.event_pkt_a.twtts_event_a.eventAll[0];
                            //                                                                event.eventAll[1] = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.event_pkt_a.twtts_event_a.eventAll[1];
                            //                                                                event.eventAll[2] = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.event_pkt_a.twtts_event_a.eventAll[2];
                            break;
                        case EvtPktBCode:
//                            sendText("Got EvtPktBCode packet\n\r", 1, 0);
                            thread_wait(20);
                            //copy 3 bytes (EvtPktBCode)
                            //					        event.eventAll[3] = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.event_pkt_b.twtts_event_b.eventAll[0];
                            //				            event.eventAll[4] = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.event_pkt_b.twtts_event_b.eventAll[1];
                            //				            event.eventAll[5] = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.event_pkt_b.twtts_event_b.eventAll[2]; 
                            //					        store_event(&event);
                            break;
                            //
                            //set thermostat settings
                            //
                        case TstatTemperatureDisplayPktACode:
                            //					        tstatPktA = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.tstat_temperature_display_pkt_a;
                            break;
                        case TstatTemperatureDisplayPktBCode:
                            //					        tstatPktB = &pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.tstat_temperature_display_pkt_b;
                            //					        //set the thermostats current temperature
                            //					        set_tstat_temp(tstatPktB->tStatNumber, tstatPktA.tStatTemp);
                            //					        //set celsius or farenheit
                            //					        set_temp_scale(tstatPktB->tStatNumber, tstatPktA.tStatDegC);
                            //					        //set the thermostats cooling setpoint
                            //					        set_tstat_cooling_pt(tstatPktB->tStatNumber, tstatPktB->tStatCoolingSetPoint);
                            //					        //set the thermostats heating setpoint
                            //					        set_tstat_heating_pt(tstatPktB->tStatNumber, tstatPktB->tStatHeatingSetPoint);
                            //					        //set the thermostats fan stat
                            //					        set_tstat_fan(tstatPktB->tStatNumber, tstatPktB->tStatFanState);
                            //					        //set the thermostats mode
                            //					        set_tstat_mode(tstatPktB->tStatNumber,tstatPktB->tStatMode);
                            //					        //set the thermostats schedule enable/disable
                            //					        set_tstat_schedule(tstatPktB->tStatNumber,tstatPktB->tStatSchedules);
                            //					        main_display_thermostat( vData.panel_status.panel_in_alarm,0);
                            break;
                            //
                            //set weather settings
                            //
                        case WeatherForcastPktACode:
                            //					        weatherPktA = pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.weather_forecast_pkt_a;
                            break;
                        case WeatherForcastPktBCode:
                            //					        setWeather_Data(  &weatherPktA,
                            //					                        &(pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.weather_forecast_pkt_b)
                            //					                     );
                            break;
                            //
                            //set service dealer contact information
                            //          or
                            //set service provider message
                            //          or
                            //set faq provide message        
                            //          or
                            //set advirtisement1-12 message
                            //          or
                            //set equipment list message
                        case DealerMessageTextType5PktCode:
                        case DealerMessageTextType6PktCode:
                            //					    case DealerMessageTextFaqPktCode:         //frequently asked questions script string
                        case DealerMessageAD1PktCode:
                        case DealerMessageAD2PktCode:
                        case DealerMessageAD3PktCode:
                        case DealerMessageAD4PktCode:
                        case DealerMessageAD5PktCode:
                        case DealerMessageAD6PktCode:
                        case DealerMessageAD7PktCode:
                        case DealerMessageAD8PktCode:
                        case DealerMessageAD9PktCode:
                        case DealerMessageAD10PktCode:
                        case DealerMessageAD11PktCode:
                        case DealerMessageAD12PktCode:
                        case DealerMessageAD13PktCode: //this maps to DEALER_PROMO_AD
                        case EquipMentListPktCode:
                        case RunScriptCmdCode:
//                            set_txt_msg(pevent_msg->message_info.rf_packet_data.rf_message.type,
//                                    &pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.message_text_pkt
//                                    );
                            break;
                        case PassThroughPktCode:
                            //only pass through supported is setting dealer logo
                            if (pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.pass_through_pkt.msg_type == 1) 
                            {
//                                set_logo(pevent_msg->message_info.rf_packet_data.rf_message.rf_message_data.pass_through_pkt.dbytea);
                            }
                            break;
                        case CodeResponsePktCode:
//                            sendText("Got CodeResponsePktCode packet\n\r", 1, 0);
                            thread_wait(20);
                            UI_message_put(pevent_msg);
                            //pevent_msg->tag = msg_valid_user_code;
                            //send valid keycode entered to uimanager;
                            //ui_message_put(pevent_msg);					        
                            break;
                    }

            }// switch	
        }// event_msg
        		if(timers_seconds_elapsed(sec_tic))
        		{
        		   sec_tic = timers_seconds_get();
        		   update_sys_timers();
        		}
        LED_Off(LED_D7);
        context_switch(); //no message received so sleep for 100mS second - allow other threads to run
    }
}

/**
 *
 *  The panel will send text number event information in response to human interaction (keypresses) to
 *  the panel. The text number will be processed and the associated data structures will be updated.
 *
 */
enum Keypress_Values parse_txt_number(struct TxtTokenDisplayPkt token_pkt) 
{
    enum Keypress_Values key_received = key_not;

    switch (token_pkt.displayToken) 
    {
            //sensor related faults
            //context_data = zone number-1 (ranges from 0 to 39)
        case T_SensorFail:
        case T_LowBat:
        case T_ChkInFault:
        case T_OpenSensor:
        case T_TamperSensor:
            /*
            case T_SensorBypass:                      //TBD what to do with bypass string?
             */
            //set bit to indicate sensor is causeing a protest
            //         if(token_pkt.tokena < NUMBER_OF_ZONES)
            //         {
            //            protest_info.protest_source.zones[token_pkt.tokena >> 3] |= token_pkt.tokena & 7;  //>>3 => divide by 8
            //         }   
            break;
            //system related faults
            //contaxt_data = is not used
        case T_AcPowerFail:
            //         protest_info.protest_source.AcPowerFail = 1;
            break;
        case T_SystemBatFail:
            //         protest_info.protest_source.SystemBatFail = 1;
        case T_ReportingFail:
            //         protest_info.protest_source.ReportingFail = 1;
            break;
        case T_PhoneLineFail:
            //         protest_info.protest_source.PhoneLineFail = 1;
            break;
        case T_IntSirenTrbl:
            //         protest_info.protest_source.IntSirenTrbl = 1;
            break;
        case T_ExtSirenTrbl:
            //         protest_info.protest_source.ExtSirenTrbl = 1;  
            break;
        case T_EnterCode:
            key_received = key_pseudo_keypad;
            break;
        default:
            break;
    }
    return key_received;
}


