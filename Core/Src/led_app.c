/*
 * led_app.c
 *
 *  Created on: Mar 18, 2020
 *      Author: felix
 */
#include "led_app.h"
#include "led_test.h"

uint32_t notif;

static void unsetNotification(uint32_t notification) {
	notif &= (uint32_t) (~notification);
}

static void setNotification(uint32_t notification) {
	notif |= notification;
}

void initializeLEDApplication(TIM_HandleTypeDef *htim1,
		TIM_HandleTypeDef *htim2) {
	/* Initializing the LED stripe stuctures */
	Bus1_LEDStripe.timer = htim1;
	Bus1_LEDStripe.is_animating = false;
	Bus1_LEDStripe.spi_bus = bus_1;
	Bus1_LEDStripe.queue = malloc(sizeof(led_pattern_queue_t));
	Bus1_LEDStripe.queue->head = NULL;
	Bus1_LEDStripe.is_timer_active = false;
	Bus1_LEDStripe.notification = LED_BUS1_NOTIF;

	Bus2_LEDStripe.timer = htim2;
	Bus2_LEDStripe.is_animating = false;
	Bus2_LEDStripe.spi_bus = bus_2;
	Bus2_LEDStripe.queue = malloc(sizeof(led_pattern_queue_t));
	Bus2_LEDStripe.queue->head = NULL;
	Bus2_LEDStripe.is_timer_active = false;
	Bus2_LEDStripe.notification = LED_BUS2_NOTIF;
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

static void clearQueue(led_stripe_t *stripe) {
	led_pattern_t pat;
	uint8_t chk = EXIT_SUCCESS;

	debug_log("Clearing queue of stripe #%d now ...", stripe->spi_bus + 1);
	while (chk == EXIT_SUCCESS) {
		chk = popQueueElement(stripe->queue, &pat);
	}
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

static void stopTimer(led_stripe_t *stripe){
	debug_log("stopping timer of stripe #%d", stripe->spi_bus + 1);
	HAL_TIM_Base_Stop_IT(stripe->timer);
	stripe->is_timer_active = false;
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
	case no_animation:
		return EXIT_FAILURE;
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
	if (stripe->is_animating) {
		debug_log("Stopping animation...");
		clearQueue(stripe);
		free(stripe->animation_pattern.led_colors);
		stopTimer(stripe);
	}
	stripe->is_animating = false;
}

static void appendLEDCommandToQueue(led_cmd_t *cmd) {
	debug_log("Appending cmd to msg queue");
	led_stripe_t *stripe = NULL;
	led_pattern_t pat;

	if (cmd->bus_num == Bus1_LEDStripe.spi_bus) {
		stripe = &Bus1_LEDStripe;
	} else if (cmd->bus_num == Bus2_LEDStripe.spi_bus) {
		stripe = &Bus2_LEDStripe;
	}

	pat.direction = cmd->ani_dir;
	pat.duration_ms = cmd->duration_ms;
	pat.led_num = cmd->led_num;
	pat.led_colors = cmd->led_colors;

	stopAnimating(stripe);

	pushQueueElement(stripe->queue, &pat);

	if (cmd->ani_dir != no_animation) {
		clearQueue(stripe);
		startAnimating(stripe, &pat);
	}

	if (stripe->is_timer_active == false) {
		debug_log("Timer not active, notifying now.");
		setNotification(stripe->notification);
	}
}

uint8_t handleLEDCommandToApplication(uint8_t *buf, uint32_t len) {
	led_cmd_t cmd = { 0 };
	cmd.led_num = buf[3] << 8 | buf[4];
	cmd.led_type = buf[5];
	cmd.ani_dir = buf[6];
	cmd.duration_ms = buf[7] << 8 | buf[8];
	cmd.bus_num = buf[9];
	uint16_t ledByteWidth = LED_BYTE_WIDTH(cmd.led_type);

	if (cmd.led_num * ledByteWidth != (len - LED_COMMAND_HEADER_LENGTH)) {
		EXIT_FAILURE;
	}

	cmd.led_colors = malloc(cmd.led_num * sizeof(led_rgb_color_t));
	uint16_t cursor = LED_COMMAND_HEADER_LENGTH;
	for (int i = 0; i < cmd.led_num; i++) {
		memcpy(&cmd.led_colors[i], &buf[cursor], ledByteWidth);
		cmd.led_colors[i].type = cmd.led_type;
		cursor += ledByteWidth;
	}
	appendLEDCommandToQueue(&cmd);
	return EXIT_SUCCESS;
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

void runLEDApplication() {
	debug_log("LED scheduler started ...");
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
