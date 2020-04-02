/*
 * usb_packet.h
 *
 *  Created on: Mar 31, 2020
 *      Author: felix
 */
#ifndef INC_USB_PACKET_H_
#define INC_USB_PACKET_H_

#include "led_app.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "debug_log.h"
#include "usbd_cdc_if.h"

#define LED_COMMAND_HEADER_LENGTH			(0x08)
#define LED_BYTE_WIDTH(LED_TYPE)			(3 + LED_TYPE)

typedef enum {
	led_cmd = 0,
	button_press = 1
} usb_packet_type_id;

typedef struct {
	uint16_t led_num;
	uint8_t led_type;
	uint8_t ani_dir;
	uint16_t duration_ms;
	uint8_t bus_num;
	led_rgb_color_t *led_colors;
} led_cmd_t;

typedef union{
	led_cmd_t led_cmd;
} usb_packet_type_t;

typedef struct{
	uint8_t type;
	usb_packet_type_t *packet_type;
} usb_packet_t;

void processUSBCallback(uint8_t *buf, uint32_t *len);

#endif /* INC_USB_PACKET_H_ */
