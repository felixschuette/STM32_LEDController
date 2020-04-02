/*
 * les_test.c
 *
 *  Created on: Mar 26, 2020
 *      Author: felix
 */
#include "led_test.h"
#include "usb_packet.h"

uint8_t test_msg_frame_header[] = {0x00, 0x00, 0x0F, 0x01, 0x00, 0x01, 0xf4, 0x01};

uint8_t firstLED[4] = {0xFF, 0x00, 0x00, 0x00};

void testRoutine(led_stripe_t *stripe, uint16_t ledNum) {
	debug_log("starting test routine ...");
	uint16_t ledBytesWidth = LED_BYTE_WIDTH(1);
	uint8_t *testMsgFrame = malloc((LED_COMMAND_HEADER_LENGTH + ledNum * ledBytesWidth));
	memcpy(testMsgFrame, &test_msg_frame_header, LED_COMMAND_HEADER_LENGTH);
	memcpy(testMsgFrame + LED_COMMAND_HEADER_LENGTH, firstLED, ledBytesWidth);
	memset(testMsgFrame + LED_COMMAND_HEADER_LENGTH + ledBytesWidth, 0x01, ledNum * ledBytesWidth - ledBytesWidth);
	uint8_t msgLength = LED_COMMAND_HEADER_LENGTH + ledNum * ledBytesWidth;
	processUSBCallback(testMsgFrame, (uint32_t *) &msgLength);
	runLEDScheduler();
}
