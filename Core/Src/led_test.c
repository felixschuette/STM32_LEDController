/*
 * les_test.c
 *
 *  Created on: Mar 26, 2020
 *      Author: felix
 */
#include "led_test.h"

void testRoutine(led_stripe_t *stripe, uint16_t led_num) {
	debug_log("starting test routine ...");

	led_rgb_color_t *led = malloc(sizeof(led_rgb_color_t) * led_num);

	led[0].red = 50;
	led[0].green = 50;
	led[0].blue = 50;
	led[0].white = 50;
	led[0].type = led_rgbw;

	led[1].red = 0;
	led[1].green = 0;
	led[1].blue = 5;
	led[1].white = 5;
	led[1].type = led_rgbw;

	for (int i = 2; i < led_num; i++) {
		memcpy(&led[i], &led[1], sizeof(led_rgb_color_t));
	}

	led_pattern_t patterns = { 0 };

	patterns.led_num = led_num;
	patterns.duration_ms = 20;
	patterns.direction = 0; // can be ignored anyway
	patterns.led_colors = malloc(patterns.led_num * sizeof(led_rgb_color_t));
	memcpy(patterns.led_colors, led,
			patterns.led_num * sizeof(led_rgb_color_t));
	startAnimating(stripe, &patterns);
}
