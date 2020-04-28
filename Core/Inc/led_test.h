/*
 * led_test.h
 *
 *  Created on: Mar 26, 2020
 *      Author: felix
 */

#ifndef INC_LED_TEST_H_
#define INC_LED_TEST_H_

#include "led_app.h"
#include "usbd_cdc_if.h"

//#define TEST_ACTIVE

#ifdef TEST_ACTIVE
#define TEST_TIMER				(TIM4)
#endif

TIM_HandleTypeDef *testTimer;

void initTestTimer(TIM_HandleTypeDef *htim);
void USB_packetReceive_Test();

#endif /* INC_LED_TEST_H_ */
