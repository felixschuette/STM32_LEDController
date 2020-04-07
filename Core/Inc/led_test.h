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

void testRoutine(led_stripe_t *stripe, uint16_t led_num);

void testTimer(TIM_HandleTypeDef *htim, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#endif /* INC_LED_TEST_H_ */
