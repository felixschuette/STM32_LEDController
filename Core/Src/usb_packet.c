/*
 * usb_packet.c
 *
 *  Created on: Apr 2, 2020
 *      Author: felix
 */

#include "usb_packet.h"
#include "led_app.h"

static uint8_t processLEDCommand(uint8_t *buf, uint32_t *len) {
	if (*len < LED_COMMAND_HEADER_LENGTH) {
		return EXIT_FAILURE;
	}

	led_cmd_t cmd = { 0 };
	cmd.led_num = buf[1] << 8 | buf[2];
	cmd.led_type = buf[3];
	cmd.ani_dir = buf[4];
	cmd.duration_ms = buf[5] << 8 | buf[6];
	cmd.bus_num = buf[7];
	uint16_t ledByteWidth = LED_BYTE_WIDTH(cmd.led_type);

	if (cmd.led_num * ledByteWidth != (*len - 8)) {
		EXIT_FAILURE;
	}

	cmd.led_colors = malloc(cmd.led_num * sizeof(led_rgb_color_t));
	uint16_t cursor = LED_COMMAND_HEADER_LENGTH;
	for (int i = 0; i < cmd.led_num; i++) {
		memcpy(&cmd.led_colors[i], &buf[cursor], ledByteWidth);
		cmd.led_colors[i].type = cmd.led_type;
		cursor += ledByteWidth;
	}
	appendLEDCommandToQueue(&cmd);
	return EXIT_SUCCESS;
}

void processUSBCallback(uint8_t *buf, uint32_t *len) {
	uint8_t chk = EXIT_FAILURE;
	/* transmitting back the received message */
	CDC_Transmit_FS(buf, *len);

	switch (buf[0]) {
	case led_cmd:
		chk = processLEDCommand(buf, len);
		chk == EXIT_SUCCESS ?
				debug_log("received msg is a valid led command.") :
				debug_log("Error: invalid command length!");
		break;
	}
}
