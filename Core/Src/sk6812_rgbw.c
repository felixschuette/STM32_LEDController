/*
 * led_driver.c
 *
 *  Created on: Mar 13, 2020
 *      Author: felix
 */
#include "sk6812_rgbw.h"
#include "debug_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void initializeSPIAdapter(SPI_HandleTypeDef *hspi_1, SPI_HandleTypeDef *hspi_2) {
	memcpy(&bus1, hspi_1, sizeof(SPI_HandleTypeDef));
	memcpy(&bus2, hspi_2, sizeof(SPI_HandleTypeDef));

	HAL_SPI_MspInit(&bus1);
	HAL_SPI_MspInit(&bus2);
}

static void sendSymbolStreamOnLine(uint8_t *symbols, uint16_t length,
		spi_bus_num_t busNum) {
	SPI_HandleTypeDef hspi;

	if (busNum == bus_1) {
		hspi = bus1;
	} else if (busNum == bus_2) {
		hspi = bus2;
	}

	HAL_SPI_Transmit_DMA(&hspi, symbols, length);
	free(symbols);
}

static void color2Symbol(uint8_t color, uint8_t *symbol_stream) {
	const uint8_t oneOneSymbol = 0b11001100;
	const uint8_t oneZeroSymbol = 0b11001000;
	const uint8_t zeroOneSymbol = 0b10001100;
	const uint8_t zeroZeroSymbol = 0b10001000;
	uint16_t cursor = 0;

	for (int i = 7; i >= 0; i-=2) {
		if ((color & (1 << i)) && (color & (1 << (i - 1)))) {
			memcpy(symbol_stream + cursor, &oneOneSymbol, 1);
		} else if ((color & (1 << i)) && !(color & (1 << (i - 1)))) {
			memcpy(symbol_stream + cursor, &oneZeroSymbol, 1);
		} else if (!(color & (1 << i)) && (color & (1 << (i - 1)))) {
			memcpy(symbol_stream + cursor, &zeroOneSymbol, 1);
		} else if (!(color & (1 << i)) && !(color & (1 << (i - 1)))) {
			memcpy(symbol_stream + cursor, &zeroZeroSymbol, 1);
		}
		cursor++;
	}
}

static void getColorSymbolStream(led_rgb_color_t *led, uint8_t *symbol_stream) {
	uint16_t cursor = 0;

	color2Symbol(led->green, symbol_stream + cursor);
	cursor += SYMBOL_BYTE_WIDTH;
	color2Symbol(led->red, symbol_stream + cursor);
	cursor += SYMBOL_BYTE_WIDTH;
	color2Symbol(led->blue, symbol_stream + cursor);
	if (led->type == led_rgbw) {
		cursor += SYMBOL_BYTE_WIDTH;
		color2Symbol(led->white, symbol_stream + cursor);
	}
}

static void buildSymbolStream(uint8_t **stream, led_rgb_color_t *led,
		uint16_t ledNum) {
	uint16_t stream_length = LED_STREAM_LENGTH(ledNum, led->type);
	uint32_t cursor = START_OFFSET;
	*stream = malloc(stream_length);

	memset(*stream, 0, stream_length);
	for (uint16_t i = 0; i < ledNum; i++) {
		getColorSymbolStream(led + i, (*stream) + cursor);
		cursor += SINGLE_LED_FRAME_SIZE(led->type);
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
	uint16_t stream_length = LED_STREAM_LENGTH(ledNum, led->type);

	buildSymbolStream(&symbols_stream, led, ledNum);
	sendSymbolStreamOnLine(symbols_stream, stream_length, busNum);
}
