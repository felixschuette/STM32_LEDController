/*
 * led_app.c
 *
 *  Created on: Mar 18, 2020
 *      Author: felix
 */
#include "led_app.h"

led_pattern_queue_t Bus1_LEDStripeQueue = { };
led_pattern_queue_t Bus2_LEDStripeQueue = { };
uint32_t notif;

void initializeLEDApplication(TIM_HandleTypeDef *htim1,
		TIM_HandleTypeDef *htim2) {
	Bus1_LEDStripeQueue.head = NULL;
	Bus2_LEDStripeQueue.head = NULL;

	memcpy(&bus1_timer, htim1, sizeof(TIM_HandleTypeDef));
	memcpy(&bus2_timer, htim2, sizeof(TIM_HandleTypeDef));
}

//static bool isTimerActive(uint8_t timer_id) {
//	bool isTimerActive = false;
//
//	switch (timer_id) {
//	case (LED_BUS1_TIMER):
//		timer_1.isActive == True ? isTimerActive = true : isTimerActive = false;
//		break;
//	case (LED_BUS2_TIMER):
//
//		break;
//	}
//
//	return isTimerActive;
//}

static uint8_t popQueueElement(led_pattern_queue_t *queue,
		led_pattern_t *pattern) {
	led_stripe_queue_element_t *old_head = NULL;

	if (queue->head == NULL) {
		return EXIT_FAILURE;
	}

	memcpy(pattern, &(queue->head)->led_pattern, sizeof(led_pattern_t));
	old_head = (led_stripe_queue_element_t*) queue->head;

	if (queue->head->next != NULL) {
		queue->head = (led_stripe_queue_element_t*) queue->head->next;
	} else {
		queue->head = NULL;
	}

	free(old_head);

	return EXIT_SUCCESS;
}

static uint8_t pushQueueElement(led_pattern_queue_t *queue,
		led_pattern_t *pattern) {
	led_stripe_queue_element_t *cursor = queue->head;
	led_stripe_queue_element_t *new_element = NULL;

	if (queue->head == NULL) {
		new_element = (led_stripe_queue_element_t*) malloc(
				sizeof(led_stripe_queue_element_t));
		new_element->next = NULL;
		memcpy(&new_element->led_pattern, pattern, sizeof(led_pattern_t));
		queue->head = new_element;
		return EXIT_SUCCESS;
	}

	while (cursor->next != NULL) {
		cursor = (led_stripe_queue_element_t*) cursor->next;
	}

	if (cursor->next == NULL) {
		new_element = (led_stripe_queue_element_t*) malloc(
				sizeof(led_stripe_queue_element_t));
		new_element->next = NULL;
		memcpy(&new_element->led_pattern, pattern, sizeof(led_pattern_t));
		cursor->next = (struct led_stripe_queue_element_t*) new_element;
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

void sendOneLEDColor() {
	led_rgb_color_t led[7] = { 0 };

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
	//__HAL_TIM_SET_COUNTER

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
			HAL_Delay(200);
			clearLEDs(7, bus_2);
			notif = 0;
		}
	}

}

void test_run() {
	led_rgb_color_t led[7] = { 0 };

	led[0].red = 5;
	led[0].green = 0;
	led[0].blue = 0;
	led[0].white = 0;

	led[1].red = 5;
	led[1].green = 0;
	led[1].blue = 0;
	led[1].white = 0;

	led[2].red = 5;
	led[2].green = 0;
	led[2].blue = 0;
	led[2].white = 0;

	led[3].red = 5;
	led[3].green = 0;
	led[3].blue = 0;
	led[3].white = 0;

	led[4].red = 255;
	led[4].green = 0;
	led[4].blue = 0;
	led[4].white = 0;

	led[5].red = 255;
	led[5].green = 0;
	led[5].blue = 0;
	led[5].white = 0;

	led[6].red = 255;
	led[6].green = 0;
	led[6].blue = 0;
	led[6].white = 0;

	led[7].red = 5;
	led[7].green = 0;
	led[7].blue = 0;
	led[7].white = 0;

	clearLEDs(7, bus_2);
	HAL_Delay(200);

	showLEDs(led, 8, bus_2);

//	led_pattern_t patterns[3] = { 0 };
//	patterns[0].led_num = 3;
//	patterns[0].duration_ms = 0;
//	patterns[0].direction = 0; // can be ignored anyway
//	patterns[0].led_colors = malloc(
//			patterns[0].led_num * sizeof(led_rgb_color_t));
//	memcpy(patterns[0].led_colors, led,
//			patterns[0].led_num * sizeof(led_rgb_color_t));
//	pushQueueElement(&Bus2_LEDStripeQueue, &patterns[0]);
//
//	patterns[1].led_num = 2;
//	patterns[1].duration_ms = 10;
//	patterns[1].direction = 1; // can be ignored anyway
//	patterns[1].led_colors = malloc(
//			patterns[1].led_num * sizeof(led_rgb_color_t));
//	memcpy(patterns[1].led_colors, &led[patterns[1].led_num + 1],
//			patterns[1].led_num * sizeof(led_rgb_color_t));
//	pushQueueElement(&Bus2_LEDStripeQueue, &patterns[1]);
//
//	led_pattern_t emptyElement = { 0 };
//	popQueueElement(&Bus2_LEDStripeQueue, &emptyElement);
//	showLEDs((led_rgb_color_t*) &emptyElement.led_colors, emptyElement.led_num,
//			bus_2);
//	HAL_Delay(200);
//	patterns[2].led_num = 3;
//	patterns[2].duration_ms = 25;
//	patterns[2].direction = 0; // can be ignored anyway
//	patterns[2].led_colors = malloc(
//			patterns[2].led_num * sizeof(led_rgb_color_t));
//	memcpy(patterns[2].led_colors,
//			&led[patterns[2].led_num + patterns[1].led_num + 1],
//			patterns[2].led_num * sizeof(led_rgb_color_t));
//	pushQueueElement(&Bus2_LEDStripeQueue, &patterns[2]);
//	popQueueElement(&Bus2_LEDStripeQueue, &emptyElement);
//	showLEDs((led_rgb_color_t*) &emptyElement.led_colors, emptyElement.led_num,
//			bus_2);
//	HAL_Delay(200);
//	popQueueElement(&Bus2_LEDStripeQueue, &emptyElement);
//	showLEDs((led_rgb_color_t*) &emptyElement.led_colors, emptyElement.led_num,
//			bus_2);

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == LED_BUS1_TIMER) {
		debug_log("LED_BUS1_TIMER");
	} else if (htim->Instance == LED_BUS2_TIMER) {
		debug_log("LED_BUS2_TIMER");
	}
	notif = 1;
}
