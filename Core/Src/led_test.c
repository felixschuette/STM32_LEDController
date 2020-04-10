/*
 * les_test.c
 *
 *  Created on: Mar 26, 2020
 *      Author: felix
 */
#include "led_test.h"
#include "usb_packet.h"

TIM_HandleTypeDef *tim;
GPIO_TypeDef *GPIO_PortX;
uint16_t GPIO_PinX;

const uint32_t on = (GPIO_PIN_10 << 0);
const uint32_t off = (GPIO_PIN_10 << 0);

uint32_t my_data_buf[] = { GPIO_PIN_10, GPIO_PIN_10 << 16, GPIO_PIN_10,
GPIO_PIN_10 << 16, GPIO_PIN_10, GPIO_PIN_10 << 16, GPIO_PIN_10,
GPIO_PIN_10 << 16 };
uint32_t my_data_buf_length = sizeof(my_data_buf);

//uint8_t dma_test_frame[] = {1 << 0, 1 << 16}

void testRoutine(led_stripe_t *stripe, uint16_t ledNum) {
	debug_log("starting test routine ...");
	uint8_t test_header[] = { 0, 4, 60, 0, 51, 1, 0, 0, 200, 1 };
	uint8_t test_frame_1[] = { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
			1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
			0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };
	uint8_t test_frame_2[] = { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
			1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
			0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };
	uint8_t test_frame_3[] = { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
			1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
			0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };
	uint8_t test_frame_4[] = { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
			1, 1, 1, 0, 3, 3, 3, 3 };

	uint32_t header_length = sizeof(test_header);
	uint32_t normal_frame_length = sizeof(test_frame_1);
	uint32_t last_frame_length = sizeof(test_frame_4);
	USB_packetRxCallback(test_header, &header_length);
	HAL_Delay(50);
	USB_packetRxCallback(test_frame_1, &normal_frame_length);
	HAL_Delay(50);
	USB_packetRxCallback(test_frame_2, &normal_frame_length);
	HAL_Delay(50);
	USB_packetRxCallback(test_frame_3, &normal_frame_length);
	HAL_Delay(50);
	USB_packetRxCallback(test_frame_4, &last_frame_length);
	HAL_Delay(50);
	runLEDApplication();
}

void dma_gpio_switch() {
	static uint32_t pinState = 1024;
	uint32_t set = GPIO_PIN_10 << 0;
	uint32_t unset = ((uint32_t) GPIO_PIN_10) << 16u;
	/* 6 (Enable DMA) */
	if (pinState == unset) {
		HAL_DMA_Start_IT(tim->hdma[TIM_DMA_ID_UPDATE], (uint32_t) &my_data_buf,
				(uint32_t) &GPIOB->ODR, my_data_buf_length);
//			HAL_DMA_Start_IT(tim->hdma[TIM_DMA_ID_UPDATE], (uint32_t) &my_data_buf, (uint32_t) &GPIO_PortX->BSRR, my_data_buf_length);
		//GPIO_PortX->BSRR = set;
		pinState = set;
		debug_log("SET");
	} else if (pinState == set) {
		HAL_DMA_Start_IT(tim->hdma[TIM_DMA_ID_UPDATE], (uint32_t) &set,
				(uint32_t) &GPIOB->ODR, sizeof(uint32_t));
//			HAL_DMA_Start_IT(tim->hdma[TIM_DMA_ID_UPDATE], (uint32_t) &my_data_buf, (uint32_t) &GPIO_PortX->BSRR, my_data_buf_length);
		//GPIO_PortX->BSRR = unset;
		pinState = unset;
		debug_log("RESET");
	}
	debug_log("dma transfer");
	//HAL_DMA_Start_IT(tim->hdma[TIM_DMA_ID_UPDATE], (uint32_t) &my_data_buf[0], (uint32_t) &GPIO_PortX->BSRR, my_data_buf_length);
}

void dataTransmittedHandler(DMA_HandleTypeDef *hdma) {
	/* Stop timer */
	__HAL_TIM_DISABLE(tim);
	volatile uint32_t debug_int = 0;

	debug_log("transmitting data");
	HAL_DMA_Start_IT(tim->hdma[TIM_DMA_ID_UPDATE], (uint32_t) &my_data_buf, (uint32_t) &GPIOB->BSRR, my_data_buf_length);
	/* Reconfigure DMA */
	__HAL_TIM_ENABLE(tim);
}

void transmitErrorHandler(DMA_HandleTypeDef *hdma) {
	/* Stop timer */
	__HAL_TIM_DISABLE(tim);
	debug_log("Transmit errror");
	/* Some error handle ? */
}

void testTimer(TIM_HandleTypeDef *htim, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
	debug_log("starting test timer");
	tim = htim;
	GPIO_PinX = GPIO_Pin;
	GPIO_PortX = GPIOx;

	__HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);

	htim->hdma[TIM_DMA_ID_UPDATE]->XferCpltCallback = dataTransmittedHandler;
	htim->hdma[TIM_DMA_ID_UPDATE]->XferErrorCallback = transmitErrorHandler;

	HAL_DMA_Start_IT(tim->hdma[TIM_DMA_ID_UPDATE], (uint32_t) &my_data_buf, (uint32_t) &GPIOB->BSRR, my_data_buf_length);
	/* 7 (Enable TIM for DMA events) */
	__HAL_TIM_ENABLE_DMA(tim, TIM_DMA_UPDATE);

	/* 8 (Run TIM) */
	__HAL_TIM_ENABLE(tim);
}
