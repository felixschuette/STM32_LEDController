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

	bus1_timer = htim1;
	bus2_timer = htim2;
}

static bool isTimerActive(uint8_t timer_id) {
	bool isTimerActive = false;

//	switch (timer_id) {
//	case (LED_BUS1_TIMER):
//		break;
//	case (LED_BUS2_TIMER):
//		break;
//	}

	return isTimerActive;
}

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

static void startTimer(TIM_HandleTypeDef *timer, uint32_t duration_ms) {

	if (timer->Instance == LED_BUS1_TIMER) {
		debug_log("starting timer for LED_BUS1 with duration: %d", duration_ms);
	} else if (timer->Instance == LED_BUS2_TIMER) {
		debug_log("starting timer for LED_BUS2 with duration: %d", duration_ms);
	}

	__HAL_TIM_CLEAR_IT(timer, TIM_IT_UPDATE);
	__HAL_TIM_SET_AUTORELOAD(timer, duration_ms);
	HAL_TIM_Base_Start_IT(timer);
}

static uint8_t showNextPattern(led_pattern_queue_t *queue,
		TIM_HandleTypeDef *timer, spi_bus_num_t busNum) {
	led_pattern_t nextPattern = { };
	uint8_t chk = EXIT_SUCCESS;

	chk = popQueueElement(queue, &nextPattern);
	if (chk == EXIT_FAILURE) {
		debug_log("queue is empty, nothing to display ...");
		return EXIT_FAILURE;
	}

	if (nextPattern.duration_ms > 0) {
		startTimer(timer, nextPattern.duration_ms);
	}

	debug_log("showing next pattern with duration %d and #%d leds now.",
			nextPattern.duration_ms, nextPattern.led_num);
	showLEDs(nextPattern.led_colors, nextPattern.led_num, busNum); //TODO: bus number must be generated dynamically

	return chk;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == LED_BUS1_TIMER) {
		debug_log("LED_BUS1_TIMER");
		notif |= LED_BUS1_NOTIF;
	} else if (htim->Instance == LED_BUS2_TIMER) {
		debug_log("LED_BUS2_TIMER");
		notif |= LED_BUS2_NOTIF;
	}
	HAL_TIM_Base_Stop(htim);
}

static void testRoutine() {
	debug_log("starting test routine ...");
	led_rgb_color_t led[9] = { 0 };

	led[0].red = 50;
	led[0].green = 0;
	led[0].blue = 0;
	led[0].white = 0;

	led[1].red = 50;
	led[1].green = 0;
	led[1].blue = 0;
	led[1].white = 0;

	led[2].red = 50;
	led[2].green = 0;
	led[2].blue = 0;
	led[2].white = 0;

	led[3].red = 0;
	led[3].green = 50;
	led[3].blue = 0;
	led[3].white = 0;

	led[4].red = 0;
	led[4].green = 50;
	led[4].blue = 0;
	led[4].white = 0;

	led[5].red = 0;
	led[5].green = 50;
	led[5].blue = 0;
	led[5].white = 0;

	led[6].red = 0;
	led[6].green = 0;
	led[6].blue = 50;
	led[6].white = 0;

	led[7].red = 0;
	led[7].green = 0;
	led[7].blue = 50;
	led[7].white = 0;

	led[8].red = 0;
	led[8].green = 0;
	led[8].blue = 50;
	led[8].white = 0;

	led_pattern_t patterns[3] = { 0 };

	patterns[0].led_num = 3;
	patterns[0].duration_ms = 1200;
	patterns[0].direction = 0; // can be ignored anyway
	patterns[0].led_colors = malloc(
			patterns[0].led_num * sizeof(led_rgb_color_t));
	memcpy(patterns[0].led_colors, led,
			patterns[0].led_num * sizeof(led_rgb_color_t));
	pushQueueElement(&Bus2_LEDStripeQueue, &patterns[0]);

	patterns[1].led_num = 3;
	patterns[1].duration_ms = 800;
	patterns[1].direction = 1; // can be ignored anyway
	patterns[1].led_colors = malloc(
			patterns[1].led_num * sizeof(led_rgb_color_t));
	memcpy(patterns[1].led_colors, &led[patterns[1].led_num],
			patterns[1].led_num * sizeof(led_rgb_color_t));
	pushQueueElement(&Bus2_LEDStripeQueue, &patterns[1]);

	patterns[2].led_num = 3;
	patterns[2].duration_ms = 3500;
	patterns[2].direction = 0; // can be ignored anyway
	patterns[2].led_colors = malloc(
			patterns[2].led_num * sizeof(led_rgb_color_t));
	memcpy(patterns[2].led_colors,
			&led[patterns[2].led_num + patterns[1].led_num],
			patterns[2].led_num * sizeof(led_rgb_color_t));
	pushQueueElement(&Bus2_LEDStripeQueue, &patterns[2]);
	debug_log("test routine: pushed 3 elements into pattern queue.");
}

static void unsetNotification(uint32_t notification) {
	notif &= (uint32_t) (~notification);
}

void runScheduler() {
	testRoutine();
	showNextPattern(&Bus2_LEDStripeQueue, bus2_timer, bus_2);
	while (1) {
		if (notif & LED_BUS1_NOTIF) {
			debug_log("Received LED_BUS1_NOTIF");
			showNextPattern(&Bus1_LEDStripeQueue, bus1_timer, bus_1);
			unsetNotification(LED_BUS1_NOTIF);
		}
		if (notif & LED_BUS2_NOTIF) {
			debug_log("Received LED_BUS2_NOTIF");
			showNextPattern(&Bus2_LEDStripeQueue, bus2_timer, bus_2);
			unsetNotification(LED_BUS2_NOTIF);
			//TODO: delete this
			testRoutine();
		}
	}
}

