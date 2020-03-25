/*
 * debug_log.h
 *
 *  Created on: Mar 25, 2020
 *      Author: felix
 */

#ifndef INC_DEBUG_LOG_H_
#define INC_DEBUG_LOG_H_

#include "stdio.h"
#include "stdarg.h"
#include "stm32f1xx_hal.h"

uint32_t rtc_get_time_usec();

void debug_log(char *msg, ...);

#endif
