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
#include "stdlib.h"
#include "assert.h"
#include "debug_log.h"
#include "usbd_cdc_if.h"


#define USB_PACKET_TIMEOUT					(3000)
#define LED_COMMAND_HEADER_LENGTH			(0x08)
#define LED_BYTE_WIDTH(LED_TYPE)			(3 + LED_TYPE)

typedef enum {
	led_cmd = 0,
} usb_header_type_id;

typedef struct {
	uint16_t led_num;
	uint8_t led_type;
	uint8_t ani_dir;
	uint16_t duration_ms;
	uint8_t bus_num;
	led_rgb_color_t *led_colors;
} led_cmd_t;

typedef union {
	led_cmd_t led_cmd;
} usb_packet_type_t;

typedef struct {
	uint8_t type;
	usb_packet_type_t *packet_type;
} usb_packet_t;

typedef enum {
	idle = 0, receiving = 1
} usb_rx_state_t;

typedef struct {
	usb_rx_state_t state;
	usb_header_type_id packet_type;
	uint16_t num_packets;
	uint16_t packet_size;
	uint16_t packets_cnt;
	uint32_t last_receiption;
	uint8_t *buf;
	uint32_t buf_length;
	uint32_t buf_cnt;
} USB_rx_cfg_t;

USB_rx_cfg_t USB_rxConfig;

void USB_initRxConfig();

void USB_packetRxCallback(uint8_t *buf, uint32_t *len);

#endif /* INC_USB_PACKET_H_ */
