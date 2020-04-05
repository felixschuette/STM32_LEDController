/*
 * les_test.c
 *
 *  Created on: Mar 26, 2020
 *      Author: felix
 */
#include "led_test.h"
#include "usb_packet.h"

uint8_t test_msg_frame_header[] = {0x00, 0x00, 0x0F, 0x01, 0x01, 0x01, 0xf4, 0x01};

uint8_t firstLED[4] = {0xFF, 0x00, 0x00, 0x00};

void testRoutine(led_stripe_t *stripe, uint16_t ledNum) {
	debug_log("starting test routine ...");
	uint8_t test_header[] = {0, 4, 60, 0, 51, 1, 0, 0, 200, 1};
	uint8_t test_frame_1[] = {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0};
	uint8_t test_frame_2[] = {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0};
	uint8_t test_frame_3[] = {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0};
	uint8_t test_frame_4[] = {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 3, 3, 3, 3};

	uint32_t header_length = sizeof(test_header);
	uint32_t normal_frame_length = sizeof(test_frame_1);
	uint32_t last_frame_length = sizeof(test_frame_4);
	USB_packetRxCallback(test_header, &header_length);
	USB_packetRxCallback(test_frame_1, &normal_frame_length);
	USB_packetRxCallback(test_frame_2, &normal_frame_length);
	USB_packetRxCallback(test_frame_3, &normal_frame_length);
	USB_packetRxCallback(test_frame_4, &last_frame_length);
	runLEDApplication();
}
