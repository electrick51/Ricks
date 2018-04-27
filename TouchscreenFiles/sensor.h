/*****************************************************************************
* Filename: sensor.h
*  Version: 1.0.0
*
*  sensor related support
*
*
*
******************************************************************************/
#ifndef SENSOR_H
#define SENSOR_H
#define NUMBER_OF_ZONES 40          //number of zones in the panel
#define MAXNAMETOKENS 16

#include "queue.h"
#include "dlpktdef.h"

enum SENSOR_TYPE
{
    motion  = 0,
    asset   = 1,
    door    = 3,
    window  = 4,
        
    not_programmed = 0xff       
};    
enum SENSOR_PRESENT_RECHECK
{
    clear_recheck = 0,      
    request_recheck = 1,
    force_recheck = 2
};    

uint8_t get_sensor_name(uint8_t dest[], uint8_t sensor, uint8_t max);
uint8_t get_sensor_token(uint8_t sensorN, uint8_t index);
union _CHANNEL_STATUS get_channel_status(uint8_t sensorN);
void set_door_window_status(void);
void clrAlarmHistory(uint8_t clrAll);
uint8_t get_zn_text(uint8_t buf[],uint8_t max,uint16_t phraseN);
uint8_t get_sensor_text(uint8_t src[], uint8_t dest[], uint8_t max);
uint8_t is_door(uint8_t index);
uint8_t is_window(uint8_t index);
uint8_t motion_is_present(void);
uint8_t door_is_present(void);
uint8_t window_is_present(void);
uint8_t property_is_present(void);
void set_recheck_present_flags(void);
void set_sensor_text(uint32_t flash_src, enum Stream_Id sensorN);
void set_motion_present(uint8_t val);
void set_property_present(uint8_t val);
void set_sensor_groups(uint32_t flash_src);
uint8_t CheckIfWindowOrDoorOrTouchpadorMotion(uint8_t SensorID);
uint8_t get_sensor_group(uint8_t sensor);
uint8_t is_motion(uint8_t sensor);
void set_property_recheck(enum SENSOR_PRESENT_RECHECK val);
void update_chn_status(uint8_t zone, uint8_t newChnData);

#endif
