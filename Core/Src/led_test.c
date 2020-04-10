/*
 * les_test.c
 *
 *  Created on: Mar 26, 2020
 *      Author: felix
 */
#include "led_test.h"
#include "usb_packet.h"

void testRoutine(led_stripe_t *stripe, uint16_t ledNum) {
	debug_log("starting test routine ...");
	uint8_t test_header[] = { 0, 2, 60, 0, 20, 1, 1, 7, 208, 1 };
	uint8_t test_frame_1[] = { 25, 0, 0, 0, 50, 0, 0, 0, 75, 0, 0, 0, 100, 0, 0,
			0, 125, 0, 0, 0, 150, 0, 0, 0, 175, 0, 0, 0, 200, 0, 0, 0, 225, 0,
			0, 0, 250, 0, 0, 0, 250, 25, 0, 0, 250, 50, 0, 0, 250, 75, 0, 0,
			250, 100, 0, 0, 250, 125, 0, 0 };
	uint8_t test_frame_2[] = { 250, 150, 0, 0, 250, 175, 0, 0, 250, 200, 0, 0,
			250, 225, 0, 0, 250, 250, 0, 0 };
	uint32_t header_length = sizeof(test_header);
	uint32_t normal_frame_length = sizeof(test_frame_1);
	uint32_t last_frame_length = sizeof(test_frame_2);

	USB_packetRxCallback(test_header, &header_length);
	HAL_Delay(50);
	USB_packetRxCallback(test_frame_1, &normal_frame_length);
	HAL_Delay(50);
	USB_packetRxCallback(test_frame_2, &last_frame_length);
	HAL_Delay(50);
	runLEDApplication();
}
