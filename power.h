#ifndef POWER_H
#define POWER_H

/**
 * This header provides the public interface to power management handling for
 * the Two-Way Talking Touch Screen project.
 */

//#include "twttsdef.h"

/**
 * Type declarations
 */

enum power_device
{
   PWRDEV_SUPPLY,
   PWRDEV_BATTERY,
   PWRDEV_4_5V,    /* Power supply for RF and audio. */
   PWRDEV_DISPLAY,
   PWRDEV_USB,
   PWRDEV_TOTAL
};

enum power_state
{
   POWER_OFF,
   POWER_LOW,
   POWER_NORMAL,
   POWER_TOTAL
};

/**
 * Public function prototypes
 */

void     power_set(uint8_t state, enum power_device device);
void     power_thread_init(void);
uint8_t  power_main_ok(void);
uint8_t  power_battery_ok(void);
void     power_ac_lost(void);
void     power_ac_restored(void);
void     power_battery_lost(void);
void     power_battery_restored(void);
void     power_audio_shutdown_timer_set(void);
void     power_audio_shutdown_timer_reset(void);
void     verify_power_on_start(void);
void     start_battery_tests(void);
void     check_battery_status(void);

#endif /* POWER_H */
