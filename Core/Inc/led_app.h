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

#define LED_BUS1_NOTIF					(1 << 0)
#define LED_BUS2_NOTIF					(1 << 1)

#define LED_BUS1_TIMER 			(TIM2)
#define LED_BUS2_TIMER			(TIM3)

typedef struct {
	led_rgb_color_t *led_colors;
	uint16_t led_num;
	uint8_t direction; //  0 .-> up, 1 -> down
	uint16_t duration_ms; // if set to zero, no timer will be set
} led_pattern_t;

typedef struct {
	led_pattern_t led_pattern;
	struct led_stripe_queue_element_t *next;
} led_stripe_queue_element_t;

typedef struct {
	led_stripe_queue_element_t *head;
	led_stripe_queue_element_t *tail;
} led_pattern_queue_t;

typedef struct {
	TIM_HandleTypeDef *timer;
	spi_bus_num_t spi_bus;
	led_pattern_queue_t *queue;
	bool is_animating;
	bool is_timer_active;
	led_pattern_t animation_pattern;
} led_stripe_t;

typedef enum {
	animation_upwards = 0, animation_downwards = 1
} animation_direction_t;

led_stripe_t Bus1_LEDStripe;
led_stripe_t Bus2_LEDStripe;

void initializeLEDApplication(TIM_HandleTypeDef *htim1,
		TIM_HandleTypeDef *htim2);

void startAnimating(led_stripe_t *stripe, led_pattern_t *pattern);

void stopAnimating(led_stripe_t *stripe);

void runLEDScheduler();

#endif /* SRC_LED_APP_H_ */
