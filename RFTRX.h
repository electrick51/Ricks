//
// rf header file
//
#ifndef RFTRX_H
#define RFTRX_H

#ifndef QUEUE_H
#include "queue.h"
#endif

void rf_tx_message_put(struct Message_Data *new_msg);
struct Message_Data *rf_tx_message_get(void);
void rf_message_init(void);
void rf_rx_thread(void);
void rf_rx_thread_init(void);
void rf_tx_thread(void);
void rf_tx_thread_init(void);
void enroll(void);
void enroll_abort(void);
void send_supervisory_report(void);
uint8_t get_panel_firmware_ver( void );
void send_battery_report( void );
#endif



