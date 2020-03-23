/*
 * led_driver.c
 *
 *  Created on: Mar 13, 2020
 *      Author: felix
 */
#include "sk6812_rgbw.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void initializeSPIAdapter(SPI_HandleTypeDef *hspi_1, SPI_HandleTypeDef *hspi_2) {
	memcpy(&first_hspi, hspi_1, sizeof(SPI_HandleTypeDef));
	memcpy(&second_hspi, hspi_2, sizeof(SPI_HandleTypeDef));

	HAL_SPI_MspInit(&first_hspi);
	HAL_SPI_MspInit(&second_hspi);
}

static void sendSymbolStreamOnLine(uint8_t *symbols, uint16_t length,
		spi_bus_num_t busNum) {
	SPI_HandleTypeDef hspi;

	if(busNum == bus_1){
		hspi = first_hspi;
	}
	else if(busNum == bus_2){
		hspi = second_hspi;
	}

	HAL_SPI_Transmit_DMA(&hspi, symbols, length);
	free(symbols);
}

static void color2Symbol(uint8_t color, uint8_t *symbol_stream) {
	const uint8_t oneSymbol = 0b11110000;
	const uint8_t zeroSymbol = 0b11000000;
	uint16_t cursor = 0;

	for (int i = 7; i >= 0; i--) {
		if (color & (1 << i)) {
			memcpy(symbol_stream + cursor, &oneSymbol, 1);
		} else {
			memcpy(symbol_stream + cursor, &zeroSymbol, 1);
		}
		cursor++;
	}
}

static void getColorSymbolStream(led_rgb_color_t *led, uint8_t *symbol_stream) {
	uint16_t cursor = 0;

	color2Symbol(led->green, symbol_stream + cursor);
	cursor += 8;
	color2Symbol(led->red, symbol_stream + cursor);
	cursor += 8;
	color2Symbol(led->blue, symbol_stream + cursor);
	cursor += 8;
	color2Symbol(led->white, symbol_stream + cursor);
}

static void buildSymbolStream(uint8_t **stream, led_rgb_color_t *led,
		uint16_t ledNum) {
	uint16_t stream_length = LED_STREAM_LENGTH(ledNum);
	uint32_t cursor = START_OFFSET;
	*stream = malloc(stream_length);

	memset(*stream, 0, stream_length);
	for (uint16_t i = 0; i < ledNum; i++) {
		getColorSymbolStream(led + i, (*stream) + cursor);
		cursor += SINGLE_LED_FRAME_SIZE;
	}
}

void clearLEDs(uint16_t ledNum, spi_bus_num_t busNum) {
	led_rgb_color_t *led = malloc(sizeof(led_rgb_color_t) * ledNum);
	memset(led, 0, sizeof(led_rgb_color_t) * ledNum);

	showLEDs(led, ledNum, busNum);
	free(led);
}

void showLEDs(led_rgb_color_t *led, uint16_t ledNum, spi_bus_num_t busNum) {
	uint8_t *symbols_stream = NULL;
	uint16_t stream_length = LED_STREAM_LENGTH(ledNum);

	buildSymbolStream(&symbols_stream, led, ledNum);
	sendSymbolStreamOnLine(symbols_stream, stream_length, busNum);
}
