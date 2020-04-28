/*
 * button_app.c
 *
 *  Created on: Apr 28, 2020
 *      Author: ftt
 */
#include "button_app.h"

void initializeButtonApp(GPIO_TypeDef *GPIO_Port, uint16_t GPIO_Pin){
	debug_log("Initializing button press detection app...");
	if(HAL_GPIO_ReadPin(GpioStatus.port, GPIO_Pin) == GPIO_PIN_RESET){
		GpioStatus.active = 1;
	}
	else{
		GpioStatus.active = 0;
	}
	GpioStatus.num_presses = 0;
	GpioStatus.port = GPIO_Port;
}

gpio_status_report_t getGPIOStatusReport(){
	debug_log("getting gpio status report");
	gpio_status_report_t report = {0};
	report.bytes[0] = 0x00;
	report.bytes[1] = 0x00;
	report.bytes[2] = (GpioStatus.num_presses >> 8) & 0xFF;
	report.bytes[3] = (GpioStatus.num_presses) & 0xFF;
	report.bytes[4] = GpioStatus.active;
	GpioStatus.num_presses = 0;
	return report;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(HAL_GPIO_ReadPin(GpioStatus.port, GPIO_Pin) == GPIO_PIN_RESET){
		GpioStatus.active = 0;
		GpioStatus.num_presses ++;
		debug_log("GPIO_INACTIVE");
	}
	else{
		GpioStatus.active = 1;
		debug_log("GPIO_ACTIVE");
	}
}
