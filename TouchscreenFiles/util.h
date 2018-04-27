#ifndef UTIL_H
#define UTIL_H

/*******************************************************************************
 * This header provides the public interface to general-purpose utility
 * functions.
 */

#include "exactint.h"

/**
 * Public function prototypes
 */

void     force_restart(void);
uint16_t ema_filter(uint16_t average, uint16_t value);

#endif /* UTIL_H */
