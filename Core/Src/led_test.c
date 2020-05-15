/*
 * les_test.c
 *
 *  Created on: Mar 26, 2020
 *      Author: felix
 */
#include "led_test.h"
#include "usb_packet.h"

void startTestTimer() {
	debug_log("starting test timer ...");
	__HAL_TIM_CLEAR_IT(testTimer, TIM_IT_UPDATE);
	HAL_TIM_Base_Start_IT(testTimer);
}

void stopTestTimer() {
	HAL_TIM_Base_Stop_IT(testTimer);
}

void initTestTimer(TIM_HandleTypeDef *htim) {
	testTimer = htim;
	startTestTimer();
}

void USB_packetReceive_Test() {
	debug_log("USB_packetReceive_Test ...");

	uint8_t test_header[] = { 1, 0, 0 };
	uint32_t header_length = sizeof(test_header);

	USB_packetRxCallback(test_header, &header_length);

	debug_log("Starting timer again");
	HAL_TIM_Base_Start_IT(testTimer);
}
