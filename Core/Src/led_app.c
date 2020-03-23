/*
 * led_app.c
 *
 *  Created on: Mar 18, 2020
 *      Author: felix
 */
#include "led_app.h"

led_pattern_queue_t firstBusQueue = { };
led_pattern_queue_t secondBusQueue = { };
uint32_t notif;

void initializeLEDApplication() {
	firstBusQueue.head = NULL;
	secondBusQueue.head = NULL;

	//clearLEDs(250, bus_2);
}

static uint8_t popQueueElement(led_pattern_queue_t *queue,
		led_pattern_queue_element_t *element) {

	if (queue->head == NULL) {
		return EXIT_FAILURE;
	}

	element = queue->head;

	if (element->next != NULL) {
		queue->head = (led_pattern_queue_element_t*) queue->head->next;
	} else {
		queue->head = NULL;
	}
	return EXIT_SUCCESS;
}

static uint8_t appendToQueue(led_pattern_queue_t *queue,
		led_pattern_queue_element_t *element) {
//	led_pattern_queue_element_t cursor = queue->head;
//
//	if (cursor == NULL) {
//		queue->head = element;
//		return EXIT_SUCCESS;
//	}
//	else{
//		while(element->next != null){
//			element = element->next->next;
//		}
//	}

	return EXIT_SUCCESS;
}

void sendOneLEDColor() {
	led_rgb_color_t led[7] = { 0 };
	led_rgb_color_t led_tmp[7] = { 0 };

	led[0].red = 5;
	led[0].green = 5;
	led[0].blue = 5;
	led[0].white = 5;

	led[1].red = 5;
	led[1].green = 5;
	led[1].blue = 5;
	led[1].white = 5;

	led[2].red = 5;
	led[2].green = 5;
	led[2].blue = 5;
	led[2].white = 5;

	led[3].red = 15;
	led[3].green = 15;
	led[3].blue = 15;
	led[3].white = 5;

	led[4].red = 75;
	led[4].green = 75;
	led[4].blue = 75;
	led[4].white = 5;

	led[5].red = 200;
	led[5].green = 200;
	led[5].blue = 200;
	led[5].white = 10;

	led[6].red = 75;
	led[6].green = 75;
	led[6].blue = 75;
	led[6].white = 5;

	led[7].red = 15;
	led[7].green = 15;
	led[7].blue = 15;
	led[7].white = 5;

//	showLEDs(led, 7, bus_2);
//	HAL_Delay(1000);
//	clearLEDs(7, bus_2);
	__HAL_TIM_SET_COUNTER

//	while (1) {
//		showLEDs(led, 7, bus_2);
//		HAL_Delay(100);
//
//		memcpy(led_tmp, led, 7 * sizeof(led_rgb_color_t));
//		memcpy(led, led_tmp + 1,
//				7 * sizeof(led_rgb_color_t) - sizeof(led_rgb_color_t));
//		led[6] = led_tmp[0];
//
//		showLEDs(led, 7, bus_2);
//		HAL_Delay(100);
//	}
	while (1) {
		if (notif & 1) {
			showLEDs(led, 7, bus_2);
			HAL_Delay(1000);
			clearLEDs(7, bus_2);
			notif = 0;
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	static uint8_t gotCalled = 0;

	if (gotCalled == 0) {
		gotCalled = 1;
	}
	if (gotCalled == 1) {
		notif = 1;
		__HAL_TIM_SET_AUTORELOAD(htim, 2000);
	}
	notif = 1;
}
