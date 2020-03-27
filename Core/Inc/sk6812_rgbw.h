/*
 * led_driver.h
 *
 *  Created on: Mar 13, 2020
 *      Author: felix
 */

#ifndef INC_SK6812_RGBW_H_
#define INC_SK6812_RGBW_H_

#include "stm32f1xx_hal.h"

#define SINGLE_LED_FRAME_SIZE		(32)
#define START_OFFSET				(80)
#define RESET_TIME					(60)
#define LED_STREAM_LENGTH(LED_NUM)  (START_OFFSET +(SINGLE_LED_FRAME_SIZE*LED_NUM) + RESET_TIME)

DMA_HandleTypeDef hdma;
SPI_HandleTypeDef bus1;
SPI_HandleTypeDef bus2;

typedef enum {
	led_rgbw,
	led_rgb
} led_type_t;

typedef struct{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t white;
}led_rgb_color_t;

typedef enum {
	bus_1 = 0,
	bus_2 = 1
}spi_bus_num_t;

void initializeSPIAdapter(SPI_HandleTypeDef *hspi_1, SPI_HandleTypeDef *hspi_2);
void clearLEDs(uint16_t ledNum, spi_bus_num_t busNum);
void showLEDs(led_rgb_color_t *led, uint16_t ledNum, spi_bus_num_t busNum);
#endif /* INC_SK6812_RGBW_H_ */
