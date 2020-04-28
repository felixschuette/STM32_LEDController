/*
 * buton_app.h
 *
 *  Created on: Apr 28, 2020
 *      Author: ftt
 */

#ifndef INC_BUTTON_APP_H_
#define INC_BUTTON_APP_H_
#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "debug_log.h"

typedef struct{
	uint16_t num_presses;
	uint8_t active;
	GPIO_TypeDef *port;
}gpio_status_report_t;

gpio_status_report_t GpioStatus;

void initializeButtonApp(GPIO_TypeDef *GPIO_Port);

#endif /* INC_BUTTON_APP_H_ */
