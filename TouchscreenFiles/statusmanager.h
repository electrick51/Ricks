//
// status thread header file
//
#include "sys_variables.h"
#ifndef STATUS_MANAGER_H
#define STATUS_MANAGER_H

#include "keypress.h"
#include "queue.h"

typedef struct
{
    uint8_t lowBat:1;         //bit 0  1 = LOW BATTERY
    uint8_t pastAlarm:1;      //bit 1  1 = PAST ALARM (alarm history)
    uint8_t bypassed:1;       //bit 2  1 = BYPASSED
    uint8_t supeFailure:1;    //bit 3  1 = SUPERVISORY
    uint8_t notRestored:1;    //bit 4  1 = SENSOR NOT RESTORED
    uint8_t inAlarm:1;        //bit 5  1 = SENSOR IN ALARM
    uint8_t tampered:1;       //bit 6  1 = SENSOR TESTED IN SENSOR TEST or
                         //           TAMPER CONDITION IN OTHER LEVELS
    uint8_t delayed:1;        //bit 7 1 = ACTIVATED BUT DISARMED BY DELAY
}SENSOR_STATUS;

struct SENSOR_STATUS
{
    uint8_t  lowBat:1;         //bit 0  1 = LOW BATTERY
    uint8_t  pastAlarm:1;      //bit 1  1 = PAST ALARM (alarm history)
    uint8_t  bypassed:1;       //bit 2  1 = BYPASSED
    uint8_t  supeFailure:1;    //bit 3  1 = SUPERVISORY
    uint8_t  notRestored:1;    //bit 4  1 = SENSOR NOT RESTORED
    uint8_t  inAlarm:1;        //bit 5  1 = SENSOR IN ALARM
    uint8_t  tampered:1;       //bit 6  1 = SENSOR TESTED IN SENSOR TEST or
                               //           TAMPER CONDITION IN OTHER LEVELS
    uint8_t  delayed:1;        //bit 7 1 = ACTIVATED BUT DISARMED BY DELAY
};
   
typedef union
{
   struct SENSOR_STATUS chnBits;
   uint8_t chnByte;
}CHANNEL_STATUS;

void status_manager_thread_init(void);
void status_message_put(struct Message_Data *new_msg);
struct Message_Data *status_message_get(void);
void status_manager_message_init(void);
enum Keypress_Values parse_txt_number(struct TxtTokenDisplayPkt token_pkt);
void clear_delay_info(void);

#endif
