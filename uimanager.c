/**
 * Filename: uimanager.c
 * Version: 1.0.0
 *
 * uimanager.c contains the ui_thread. The ui_thread
 * builds and displays the 
 * the various screens.
 *
 */
#include "twttsdef.h"
#include "threads.h"
#include "queue.h"
#include "graphics\touchscreen.h"
#include "keypadscreen.h"
#include "uimanager.h"
#include "mainscreen.h"
#include "pre_arm_screen.h"
#include "calibrationscreen.h"
#include "dealercontactscreen.h"
#include "armcountdownscreen.h"
#include "tstatscreen.h"
#include "eventscreen.h"
#include "protestscreen.h"
#include "statusscreen.h"
#include "faq_screen.h"
#include "help_screen.h"
#include "information_screen.h"
#include "text_screen.h"
#include "emergencyscreen.h"
#include "sensorscreen.h"
#include "ad_screen.h"
#include "blank_screen.h"
#include "dealer_promo_screen.h"
#include "mainscreen.h"

#include "modification_screen.h"
#include "programming_screen.h"
#include "programming_access_codes_screen.h"
#include "programming_security_screen.h"
#include "programming_phone_numbers_screen.h"
#include "programming_phone_options_screen.h"
#include "programming_sensors_screen.h"
#include "programming_sensors_screen_l2.h"
#include "programming_sensors_screen_l3.h"
#include "programming_sensors_modify_name_screen.h"
#include "programming_sensors_learn_screen.h"
#include "programming_sensors_learn_screen_l2.h"
#include "programming_reporting_screen.h"
#include "programming_reporting_options_screen.h"
#include "programming_communication_modes_screen.h"
#include "programming_timers_screen.h"
#include "programming_touchpad_options_screen.h"
#include "programming_system_options_screen.h"
#include "programming_siren_options_screen.h"
#include "programming_audio_verification_screen.h"
#include "programming_light_control_screen.h"
#include "programming_system_tests_screen.h"
#include "xprintf.h"
#include "interactive_services_screen.h"
#include "directbypass_screen.h"
#include "programming_sensor_test_screen.h"
#include "programming_test_results_screen.h"
#include "programming_set_date_time_screen.h"
#include "programming_pleasewait_screen.h"
#include "photograph_screen.h"
#include "photograph_screen_L2.h"
#include "sensor_photograph_screen.h"

static void ui_manager_thread(void);

#define UI_QSIZE 10
static struct message_Q ui_message_Q;				//container information... number of messages in que.
static struct Message_Data ui_Q[UI_QSIZE];	        //declare message storage array
#define MAX_SCREEN_DEPTH 10
static struct SCREEN_STACKUP screenStackup[MAX_SCREEN_DEPTH];
static uint8_t screenLevel;

/**
 *
 *  initialize the ui_manager thread
 *
 */
void ui_manager_thread_init(void)
{
    static THREAD ui_manager_thread_struct;
    static uint8_t ui_manager_thread_stack[3200];    // TBD determine needed size 
    
    //ui_manager thread initialize();
    thread_create(ui_manager_thread, &ui_manager_thread_struct, ui_manager_thread_stack, sizeof(ui_manager_thread_stack));
}

/**
 *
 * method to put messages into ui thread queue
 *
 */
void ui_message_put(struct Message_Data *new_msg)
{	
    if (new_msg->tag == msg_keypress)
    {    
        updateBacklightBrightnessStatus(eRestoreLcdLedBrightnessOnKeypress);   //force back light to full brightness
    }    
    message_Q_put(&ui_message_Q, new_msg);
}

/**
 *
 * method to get events from the ui's message queue 
 *
 */
struct Message_Data *ui_message_get(void)
{
	return message_Q_get(&ui_message_Q);
}

/**
 *
 * initialize the ui threads message queue
 *
 */
void uimanager_message_init(void)
{
	message_Q_initialize(&ui_message_Q,ui_Q, UI_QSIZE);    	//initialize message q.    
}

/**
 *
 * force screen focus to the sc_main_screen... an alarm has occurred etc.
 * returns: 0 retain focus
 *          1 return focus   
 *
 * screens must reliquish control for the following reasons:  
 * 1) panel has entered alarm
 * 2) panel in entry delay
 *
 */

 /*
uint8_t relinquish_focus(void)
{
    extern struct VDATA vData;

    if(vData.panel_status.intrusion_alarm || vData.panel_status.emergency_alarm || vData.panel_status.fire_alarm || vData.entry_delay)
        return 1;
        
    

    return 0;       
} */

uint8_t relinquish_focus(uint16_t timeoutStart, uint16_t timeoutSeconds)
{
    static uint16_t progmode_tm_out_start;
    extern struct VDATA vData;
    struct PANELDATA panelData;
    //
    //check if touchscreen is in program mode menus
    //
    if (vData.inProgramModeScreens)
    {
        if(timers_seconds_elapsed(progmode_tm_out_start))
        {
            progmode_tm_out_start = timers_seconds_get();        
            panelData.inputDataLen = 0;
            if (getPanelData(tProgamModeTimer, &panelData) != eUIItemOk)
            {
                return 1;   //=>exit program menu
            }
        }
        return 0;           //ok to stay in program menu    
    }
    if (
       vData.panel_status.intrusion_alarm || 
       vData.panel_status.emergency_alarm || 
       vData.panel_status.fire_alarm || 
       vData.entry_delay || 
	   vData.panel_status.panel_in_alarm ||
       ((timeoutSeconds != 0) && (timers_seconds_elapsed(timeoutStart) >= timeoutSeconds))
       )
        return 1;
    return 0;       
}

uint8_t ScreenStackupDifference(struct SCREEN_STACKUP *screenA, struct SCREEN_STACKUP *screenB)
{
    if (screenA->screenToDisplay != screenB->screenToDisplay)
        return 1;
    if (screenA->rowNumber != screenB->rowNumber)
        return 1;       
    return 0;
}

void ClearOneStackupEntry(struct SCREEN_STACKUP *screenEntry)
{
    memFill((uint8_t*)screenEntry,0,sizeof(struct SCREEN_STACKUP));
//    screenEntry->screenToDisplay = 0;
//    screenEntry->rowNumber = 0;    
}

/**
 Function:    ClearMultipleStackupEntries
 Parameters:  *screenEntry = pointer to data of one level of screen stackup
              totalEntries = how many levels of screen stackup you wish to clear
 Returns:     none
 Description: Clears the given number of screen stackup levels
 Notes:       README (mandatory for first time users)
              This function will clear totalEntries number of stackup levels BACKWARDS from the *screenEntry pointer.
              I feel it makes sense to do it this way as how it is currently being used, though it may (will) casue problems
              for people who have never used this function before.
*/
void ClearMultipleStackupEntries(struct SCREEN_STACKUP *screenEntry, uint8_t totalEntries)
{
    uint8_t i;
    
    for (i = 0;i < totalEntries;i++)
    {
        memFill((uint8_t*)&screenEntry[-i],0,sizeof(struct SCREEN_STACKUP));
    }
}

/**
 *
 *  The ui thread navigates the screen flow building and displaying
 *  screens as required.
 *
 */	
static void ui_manager_thread(void)
{
    struct LOCATION location;    
    static struct SCREEN_STACKUP /*screenInFocus, */newScreenInFocus, lastScreenInFocus;
    
    struct SCREEN_STACKUP *pCurrentScreen;
    //
    // Things to do:
    //   1. check for messages in Q
    //	
    //
    //
    // IO paths:
    //  
    //   
    // 
    //
	screenLevel = 0;
    screenStackup[screenLevel].screenToDisplay = /*sc_programming_screen;//*/sc_main_screen;
    screenStackup[screenLevel].rowNumber = 0;
    // Check for TP press on startup
    // get the raw location
    location = Graphics_GetUncalTouchpad(0);
    if(location.X < 0x1255 && location.Y < 0x1290)
    {
        screenLevel++;
        screenStackup[screenLevel].screenToDisplay = sc_calibration_screen;
        screenStackup[screenLevel].rowNumber = 0;
    }
    else
    {       
        lastScreenInFocus.screenToDisplay = screenStackup[screenLevel].screenToDisplay+1;         
    }           
    //last_screen_in_focus = new_screen_in_focus+1;   //forcing values to be different will force a screen redraw below
    for(;;)								            //threads should not return
	{
        pCurrentScreen = &screenStackup[screenLevel];

        //
        // see if please wait screen should be displayed
        //
//        newScreenInFocus = programming_pleasewait_screen(pCurrentScreen, ui_message_get);
//        switch(screenStackup[screenLevel].screenToDisplay)
//		{   
//            case sc_ad_screen:
//                newScreenInFocus = ad_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_armcountdwn_screen:
//                newScreenInFocus = armcountdwn_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);                
//                break;
//            /*case sc_armcountdwn_w_bypass_screen:
//                newScreenInFocus = armcountdwn_screen(lastScreenInFocus.screenToDisplay != screenStackup[screenLevel].screenToDisplay,sc_armcountdwn_screen,&screenStackup[screenLevel],ui_message_get);                
//                break;*/
//            case sc_blank_screen:
//                newScreenInFocus = blank_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_calibration_screen:
//                newScreenInFocus = calibration_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_dealer_contact_screen:
//                newScreenInFocus = dealer_contact_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_dealer_promo_screen:
//                newScreenInFocus = dealer_promo_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_doorlock_screen:
//                newScreenInFocus = doorlock_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_emergency_screen:
//                newScreenInFocus = emergency_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_event_screen:
//                newScreenInFocus = event_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_faq_screen:
//                newScreenInFocus = faq_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_help_screen:
//                newScreenInFocus = help_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            /*case sc_help_screen_b:                
//                newScreenInFocus = help_screen(sc_back_one_screen,&screenStackup[screenLevel],ui_message_get);                        
//                break;*/
//            case sc_information_screen:
//                newScreenInFocus = information_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);               
//                break;
//            case sc_keypad_screen:
//                newScreenInFocus = keypad_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//#ifdef QC375_FIX                
//                if (screenLevel != 0)                   //after using the keypad screen always remove it from the screen-stack-up
//                {
//                    screenLevel--;
//                }
//#endif
//                break;
//            /*case sc_keypad_screen_entry_delay: 
//                newScreenInFocus = keypad_screen(sc_keypad_screen_entry_delay,&screenStackup[screenLevel],ui_message_get);
//                break;*/
//            //case sc_keypad_screen_doorlock:
//            //    new_screen_in_focus = keypad_screen(sc_back_one_screen,&screenStackup[screenLevel],ui_message_get);
//            //    break;
//            case sc_light_screen:
//                newScreenInFocus = light_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//    		case sc_main_screen:
//    		    newScreenInFocus = main_screen(sc_main_screen,pCurrentScreen,ui_message_get);    		        
//    		    break;
//            case sc_pre_arm_screen:
//                newScreenInFocus = pre_arm_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_protest_screen:
//                newScreenInFocus = protest_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_sensor_screen:
//                newScreenInFocus = sensor_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_tstatus_screen:
//                //newScreenInFocus.screenToDisplay = status_screen(sc_back_one_screen,&screenStackup[screenLevel],ui_message_get);
//                newScreenInFocus = status_screen(/*lastScreenInFocus.screenToDisplay != screenStackup[screenLevel].screenToDisplay, */sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_directBypass_screen:
//                newScreenInFocus = directBypass_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;                             
//            case sc_text_screen:
//                newScreenInFocus = text_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);     
//                break; 
//            case sc_thermostat_screen:
//                newScreenInFocus = thermostat_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);    
//                break;
//            case sc_weather_screen:
//                newScreenInFocus = weather_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_modification_screen:
//                newScreenInFocus = modification_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                break;
//            case sc_programming_screen:
//                newScreenInFocus = programming_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;            
//            case sc_programming_access_codes_screen:
//                newScreenInFocus = programming_access_codes_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_security_screen:
//                newScreenInFocus = programming_security_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_phone_numbers_screen:
//                newScreenInFocus = programming_phone_numbers_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_phone_options_screen:
//                newScreenInFocus = programming_phone_options_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_sensors_screen:
//                newScreenInFocus = programming_sensors_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//                    case sc_programming_sensors_screen_L2:
//                    	newScreenInFocus = programming_sensors_screen_L2(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//						break;
//					case sc_programming_sensors_screen_L3:
//                    	newScreenInFocus = programming_sensors_screen_L3(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                      	break;
//					case sc_programming_modify_sensor_name_screen:
//                    	newScreenInFocus = programming_sensors_modify_name_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                      	break;
//                    case sc_programming_sensors_learn_screen:
//                    	newScreenInFocus = programming_sensors_learn_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                      	break;
//                    case sc_programming_sensors_learn_screen_L2:
//                    	newScreenInFocus = programming_sensors_learn_screen_l2(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                      	break;                      	
//            case sc_programming_reporting_screen:
//                newScreenInFocus = programming_reporting_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//                    case sc_programming_reporting_options_screen:
//                        newScreenInFocus = programming_reporting_options_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                        break;
//                    case sc_programming_communication_modes_screen:
//                        newScreenInFocus = programming_communication_modes_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                        break;
//            case sc_programming_timers_screen:
//                newScreenInFocus = programming_timers_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_touchpad_options_screen:
//                newScreenInFocus = programming_touchpad_options_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_system_options_screen:
//                newScreenInFocus = programming_system_options_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_siren_options_screen:
//                newScreenInFocus = programming_siren_options_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_audio_verification_screen:
//                newScreenInFocus = programming_audio_verification_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_light_control_screen:
//                newScreenInFocus = programming_light_control_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_system_tests_screen:
//                newScreenInFocus = programming_system_tests_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//                    case sc_sensor_test_screen:
//                        newScreenInFocus = programming_sensor_test_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                        break;
//                    case sc_test_result_screen:
//                        newScreenInFocus = programming_test_results_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);
//                        break;
//            case sc_interactive_services_screen:
//                newScreenInFocus = interactive_services_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_programming_set_date_time_screen:
//                newScreenInFocus = programming_set_date_time_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_photograph_screen:
//                newScreenInFocus = photograph_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_photograph_screen_L2:
//                newScreenInFocus = photograph_screen_L2(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//            case sc_sensor_photograph_screen:
//                newScreenInFocus = sensor_photograph_screen(sc_back_one_screen,pCurrentScreen,ui_message_get);            
//                break;
//                
//            default:
//                newScreenInFocus.screenToDisplay = sc_main_screen;                           //!!!this should not happen.
//                                       
//        }     
        //last_screen_in_focus = screen_in_focus; //when the screen focus changes the new screen must be redrawn
        //lastScreenInFocus.screenToDisplay = screenStackup[screenLevel].screenToDisplay;
        lastScreenInFocus = screenStackup[screenLevel];
        
        if (newScreenInFocus.screenToDisplay == sc_main_screen)
        {
            ClearMultipleStackupEntries(&screenStackup[screenLevel], screenLevel);
            screenLevel = 0;
        }        
        else if ((newScreenInFocus.screenToDisplay == screenStackup[screenLevel-1].screenToDisplay) || (newScreenInFocus.screenToDisplay == sc_back_one_screen))
        {
            if (screenLevel != 0)
            {
#ifndef QC375_FIX
                do
                {
#endif
                    ClearMultipleStackupEntries(&screenStackup[screenLevel], 1);
                    screenLevel--;
                    if (screenLevel == 0) //we should never get here, this would depict a severe error in the screen stackup
                    {
                        ClearMultipleStackupEntries(&screenStackup[MAX_SCREEN_DEPTH-1], MAX_SCREEN_DEPTH);
                        screenLevel = 0;
                        screenStackup[0].screenToDisplay = sc_main_screen;
#ifndef QC375_FIX   // gmk
                        break;
#endif
                    }
#ifndef QC375_FIX
                } while (screenStackup[screenLevel].screenToDisplay == sc_keypad_screen);
#endif
            }
            else //we should never get here, this would depict a severe error in the screen stackup
            {
                ClearMultipleStackupEntries(&screenStackup[MAX_SCREEN_DEPTH-1], MAX_SCREEN_DEPTH);
                screenLevel = 0;
                screenStackup[0].screenToDisplay = sc_main_screen;
            }
        }
        else if (newScreenInFocus.screenToDisplay != sc_same_screen)
        {
            screenLevel++;
            //screenStackup[screenLevel].screenToDisplay = newScreenInFocus.screenToDisplay;
            screenStackup[screenLevel] = newScreenInFocus;
        }
		context_switch();           //allow other threads to run
    }
}



struct GRAPHICS_METHOD getGMremoveAll()
{
    const struct GRAPHICS_METHOD removeAll =  
    {
        Control__RemoveAll,
    };
    
    return removeAll;
};
