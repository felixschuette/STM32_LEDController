/*
 * les_test.c
 *
 *  Created on: Mar 26, 2020
 *      Author: felix
 */
#include "led_test.h"

void testRoutine(led_pattern_queue_t *queue) {
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

	led[2].red = 75;
	led[2].green = 75;
	led[2].blue = 0;
	led[2].white = 0;

	led[3].red = 100;
	led[3].green = 100;
	led[3].blue = 0;
	led[3].white = 0;

	led[4].red = 75;
	led[4].green = 75;
	led[4].blue = 0;
	led[4].white = 0;

	led[5].red = 50;
	led[5].green = 0;
	led[5].blue = 0;
	led[5].white = 0;

	led[6].red = 50;
	led[6].green = 0;
	led[6].blue = 0;
	led[6].white = 0;

	led[7].red = 50;
	led[7].green = 0;
	led[7].blue = 0;
	led[7].white = 0;

	led[8].red = 50;
	led[8].green = 0;
	led[8].blue = 0;
	led[8].white = 0;

	led_pattern_t patterns = { 0 };

	patterns.led_num = 7;
	patterns.duration_ms = 1000;
	patterns.direction = 1; // can be ignored anyway
	patterns.led_colors = malloc(
			patterns.led_num * sizeof(led_rgb_color_t));
	memcpy(patterns.led_colors, led,
			patterns.led_num * sizeof(led_rgb_color_t));
	startAnimating(&Bus2_LEDStripe, &patterns);
}
