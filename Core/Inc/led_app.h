/*
 * led_app.h
 *
 *  Created on: Mar 18, 2020
 *      Author: felix
 */

#ifndef SRC_LED_APP_H_
#define SRC_LED_APP_H_

#include "stm32f1xx_hal.h"
#include "string.h"
#include "sk6812_rgbw.h"
#include "stdlib.h"

typedef struct {
	led_rgb_color_t *led_pattern;
	uint16_t led_num;
	uint16_t duration_ms; // if set to zero, no timer will be set
	struct led_pattern_queue_element_t *next;
} led_pattern_queue_element_t;

typedef struct {
	led_pattern_queue_element_t *head;
	led_pattern_queue_element_t *tail;
} led_pattern_queue_t;

void initializeLEDApplication();

void sendOneLEDColor();

#endif /* SRC_LED_APP_H_ */
