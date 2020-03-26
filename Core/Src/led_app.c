/*
 * led_app.c
 *
 *  Created on: Mar 18, 2020
 *      Author: felix
 */
#include "led_app.h"
#include "led_test.h"

uint32_t notif;

void initializeLEDApplication(TIM_HandleTypeDef *htim1,
		TIM_HandleTypeDef *htim2) {
	/* Initializing the LED stripe stuctures */
	Bus1_LEDStripe.timer = htim1;
	Bus1_LEDStripe.is_animating = false;
	Bus1_LEDStripe.spi_bus = bus_1;
	Bus1_LEDStripe.queue = malloc(sizeof(led_pattern_queue_t));
	Bus1_LEDStripe.queue->head = NULL;
	Bus1_LEDStripe.is_timer_active = false;

	Bus2_LEDStripe.timer = htim2;
	Bus2_LEDStripe.is_animating = false;
	Bus2_LEDStripe.spi_bus = bus_2;
	Bus2_LEDStripe.queue = malloc(sizeof(led_pattern_queue_t));
	Bus2_LEDStripe.queue->head = NULL;
	Bus2_LEDStripe.is_timer_active = false;
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
		Bus1_LEDStripe.is_timer_active = true;
	} else if (timer->Instance == LED_BUS2_TIMER) {
		debug_log("starting timer for LED_BUS2 with duration: %d", duration_ms);
		Bus2_LEDStripe.is_timer_active = true;
	}

	__HAL_TIM_CLEAR_IT(timer, TIM_IT_UPDATE);
	__HAL_TIM_SET_AUTORELOAD(timer, duration_ms);
	HAL_TIM_Base_Start_IT(timer);
}

static uint8_t pushAnimationPatternInQueue(led_stripe_t *stripe) {

	if (stripe->is_animating == false) {
		return EXIT_FAILURE;
	}

	led_pattern_t tmp_pattern = { };
	tmp_pattern.led_num = stripe->animation_pattern.led_num;
	tmp_pattern.direction = stripe->animation_pattern.direction;
	tmp_pattern.duration_ms = stripe->animation_pattern.duration_ms;
	tmp_pattern.led_colors = malloc(
			stripe->animation_pattern.led_num * sizeof(led_rgb_color_t));
	memcpy(tmp_pattern.led_colors, stripe->animation_pattern.led_colors,
			stripe->animation_pattern.led_num * sizeof(led_rgb_color_t));
	pushQueueElement(stripe->queue, &tmp_pattern);

	/* shift animation pattern for next time */
	switch (stripe->animation_pattern.direction) {
	case animation_upwards:
		debug_log("shifting animation upwards");
		memcpy(&stripe->animation_pattern.led_colors[1], tmp_pattern.led_colors,
				(tmp_pattern.led_num - 1) * sizeof(led_rgb_color_t));
		memcpy(stripe->animation_pattern.led_colors,
				tmp_pattern.led_colors + tmp_pattern.led_num - 1,
				sizeof(led_rgb_color_t));
		break;
	case animation_downwards:
		debug_log("shifting animation downwards");
		memcpy(stripe->animation_pattern.led_colors, &tmp_pattern.led_colors[1],
				(tmp_pattern.led_num - 1) * sizeof(led_rgb_color_t));
		memcpy(stripe->animation_pattern.led_colors + tmp_pattern.led_num - 1,
				tmp_pattern.led_colors, sizeof(led_rgb_color_t));
		break;
	}

	return EXIT_SUCCESS;
}

static uint8_t showNextPattern(led_stripe_t *stripe) {
	led_pattern_t nextPattern = { };
	uint8_t chk = EXIT_SUCCESS;

	pushAnimationPatternInQueue(stripe);

	chk = popQueueElement(stripe->queue, &nextPattern);
	if (chk == EXIT_FAILURE) {
		debug_log("queue is empty, nothing to display ...");
		return EXIT_FAILURE;
	}

	if (nextPattern.duration_ms > 0) {
		startTimer(stripe->timer, nextPattern.duration_ms);
	}

	debug_log("showing next pattern with duration %d and #%d leds n ow.",
			nextPattern.duration_ms, nextPattern.led_num);
	showLEDs(nextPattern.led_colors, nextPattern.led_num, stripe->spi_bus);
	free(nextPattern.led_colors);
	return chk;
}

void startAnimating(led_stripe_t *stripe, led_pattern_t *pattern) {
	debug_log("Starting animation ...");
	memcpy(&stripe->animation_pattern, pattern, sizeof(led_pattern_t));
	stripe->is_animating = true;
	showNextPattern(stripe);
}

void stopAnimating(led_stripe_t *stripe) {
	free(stripe->animation_pattern.led_colors);
	stripe->is_animating = false;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == LED_BUS1_TIMER) {
		debug_log("LED_BUS1_TIMER");
		Bus1_LEDStripe.is_timer_active = false;
		notif |= LED_BUS1_NOTIF;
	} else if (htim->Instance == LED_BUS2_TIMER) {
		debug_log("LED_BUS2_TIMER");
		Bus2_LEDStripe.is_timer_active = false;
		notif |= LED_BUS2_NOTIF;
	}
	HAL_TIM_Base_Stop(htim);
}

static void unsetNotification(uint32_t notification) {
	notif &= (uint32_t) (~notification);
}

void runLEDScheduler() {
	while (1) {
		if (notif & LED_BUS1_NOTIF) {
			debug_log("LED_BUS1_NOTIF");
			showNextPattern(&Bus1_LEDStripe);
			unsetNotification(LED_BUS1_NOTIF);
		}
		if (notif & LED_BUS2_NOTIF) {
			debug_log("LED_BUS2_NOTIF");
			showNextPattern(&Bus2_LEDStripe);
			unsetNotification(LED_BUS2_NOTIF);
		}
	}
}
