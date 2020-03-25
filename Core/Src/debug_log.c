/*
 * debug_loh.c
 *
 *  Created on: Mar 25, 2020
 *      Author: felix
 */
#include "debug_log.h"

uint32_t rtc_get_time_usec(){
	uint32_t tick = HAL_GetTick();
	return tick;
}

void debug_log(char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    int time = rtc_get_time_usec();
    printf("%d.%03d: ", time / 1000, time % 1000);
    vprintf(msg, args);
    printf("\n");
}
