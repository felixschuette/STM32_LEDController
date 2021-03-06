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

#define GPIO_STATUS_RESP_LEN 						(5)
#define GPIO_BOUNCING_TIME							(50)

typedef struct{
	uint16_t num_presses;
	uint8_t active;
	uint32_t rising_edge_timestamp;
	GPIO_TypeDef *port;
}gpio_status_t;

typedef struct{
	uint8_t bytes[GPIO_STATUS_RESP_LEN];
}gpio_status_report_t;

gpio_status_t GpioStatus;

void initializeButtonApp(GPIO_TypeDef *GPIO_Port, uint16_t GPIO_Pin);


gpio_status_report_t getGPIOStatusReport();

#endif /* INC_BUTTON_APP_H_ */
