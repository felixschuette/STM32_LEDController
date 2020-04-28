/*
 * button_app.c
 *
 *  Created on: Apr 28, 2020
 *      Author: ftt
 */
#include "button_app.h"

void initializeButtonApp(GPIO_TypeDef *GPIO_Port){
	debug_log("Initializing button press detection app...");
	GpioStatus.active = 0;
	GpioStatus.num_presses = 0;
	GpioStatus.port = GPIO_Port;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(HAL_GPIO_ReadPin(GpioStatus.port, GPIO_Pin) == GPIO_PIN_RESET){
		GpioStatus.active = 0;
		debug_log("GPIO_INACTIVE");
	}
	else{
		GpioStatus.active = 1;
		GpioStatus.num_presses ++;
		debug_log("GPIO_ACTIVE");
	}
}
