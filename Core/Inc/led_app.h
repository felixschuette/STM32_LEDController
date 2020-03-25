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
#include "stdio.h"
#include "stdbool.h"
#include "debug_log.h"

#define LED_BUS1_TIMER 			(TIM2)
#define LED_BUS2_TIMER			(TIM3)

TIM_HandleTypeDef bus1_timer;
TIM_HandleTypeDef bus2_timer;

typedef struct {
	led_rgb_color_t *led_colors;
	uint16_t led_num;
	uint8_t direction; //  0 .-> up, 1 -> down
	uint16_t duration_ms; // if set to zero, no timer will be set
}led_pattern_t;

typedef struct {
	led_pattern_t led_pattern;
	struct led_stripe_queue_element_t *next;
} led_stripe_queue_element_t;

typedef struct {
	led_stripe_queue_element_t *head;
	led_stripe_queue_element_t *tail;
} led_pattern_queue_t;

void initializeLEDApplication(TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2);

void sendOneLEDColor();

void test_run();

#endif /* SRC_LED_APP_H_ */
