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
#include "debug_log.h"
#include "usbd_cdc_if.h"


#define USB_PACKET_TIMEOUT					(1000)
#define LED_COMMAND_HEADER_LENGTH			(0x0A)
#define GPIO_REQ_HEADER_LENGTH				(3)
#define LED_BYTE_WIDTH(LED_TYPE)			(3 + LED_TYPE)

typedef enum {
	led_cmd = 0,
	gpio_req = 1
} usb_header_type_id;

typedef enum {
	idle = 0, receiving = 1
} usb_rx_state_t;

typedef struct {
	usb_rx_state_t state;
	usb_header_type_id packet_type;
	uint16_t num_packets;
	uint16_t packet_size;
	uint16_t packets_cnt;
	uint32_t last_reception;
	uint8_t *buf;
	uint32_t buf_length;
	uint32_t buf_cnt;
	uint8_t (*application_cb) (uint8_t *buf, uint32_t len);
} USB_rx_cfg_t;

USB_rx_cfg_t USB_rxConfig;

void USB_initRxConfig();

void USB_packetRxCallback(uint8_t *buf, uint32_t *len);

#endif /* INC_USB_PACKET_H_ */
