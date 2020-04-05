/*
 * usb_packet.c
 *
 *  Created on: Apr 2, 2020
 *      Author: felix
 */

#include "usb_packet.h"
#include "led_app.h"

void USB_initRxConfig() {
	USB_rxConfig.state = idle;
	USB_rxConfig.packet_type = 0;
	USB_rxConfig.num_packets = 0;
	USB_rxConfig.packet_size = 0;
	USB_rxConfig.packets_cnt = 0;
	USB_rxConfig.last_receiption = 0;
	USB_rxConfig.buf_length = 0;
	USB_rxConfig.buf = NULL;
	USB_rxConfig.buf_cnt = 0;
}

static void resetRxConfig() {
	debug_log("Resetting config...");
	free(USB_rxConfig.buf);
	USB_initRxConfig();
}

static bool isHeaderValid(uint8_t *buf, uint32_t len) {
	bool chk = true;
	usb_header_type_id header_type = (usb_header_type_id) buf[0];

	switch (header_type) {
	case led_cmd:
		if (len != LED_COMMAND_HEADER_LENGTH) {
			debug_log("ERROR: LED command header has invalid length");
			chk = false;
			break;
		}
		// TODO: we could do some more validity checks here
	}
	return chk;
}

static void setLEDCmdRxCfg(uint8_t *buf, uint32_t len) {
	USB_rxConfig.state = receiving;
	USB_rxConfig.packet_type = led_cmd;
	USB_rxConfig.num_packets = buf[1];
	USB_rxConfig.packet_size = buf[2];
	USB_rxConfig.packets_cnt = 0;
	USB_rxConfig.last_receiption = HAL_GetTick();
	USB_rxConfig.buf_length = len
			+ (USB_rxConfig.packet_size * USB_rxConfig.num_packets);
	USB_rxConfig.buf = malloc(USB_rxConfig.buf_length);
	USB_rxConfig.buf_cnt = len;
	memcpy(USB_rxConfig.buf, buf, len);
	USB_rxConfig.application_cb = handleLEDCommandToApplication;
	debug_log("Initialized LED command RX config ...");
}

static uint8_t setRxConfig(uint8_t *buf, uint32_t len) {
	uint8_t chk = EXIT_FAILURE;
	usb_header_type_id frame_type = (usb_header_type_id) buf[0];

	if (!isHeaderValid(buf, len)) {
		return chk;
	}

	switch (frame_type) {
	case led_cmd:
		chk = true;
		setLEDCmdRxCfg(buf, len);
		break;
	default:
		chk = false;
		break;
	}
	return chk;
}

static bool isPacketTimedOut() {
	uint32_t current_time = HAL_GetTick();

	if ((current_time - USB_rxConfig.last_receiption) > USB_PACKET_TIMEOUT) {
		resetRxConfig();
		return true;
	}
	return false;
}

static bool isPacketSizeValid(uint32_t len){
	if(len > USB_rxConfig.packet_size){
		debug_log("ERROR: packet size exceeded!");
		return false;
	}
	if((len < USB_rxConfig.packet_size) && (USB_rxConfig.packets_cnt + 1 < USB_rxConfig.num_packets)){
		debug_log("ERROR: packet size is too small");
		return false;
	}
	return true;
}

static uint8_t processNextPacket(uint8_t *buf, uint32_t len) {
	debug_log("processing next packet...");
	if (!isPacketSizeValid(len)) {
		resetRxConfig();
		return EXIT_FAILURE;
	}

	memcpy(USB_rxConfig.buf + USB_rxConfig.buf_cnt, buf, len);
	USB_rxConfig.buf_cnt += len;
	USB_rxConfig.packets_cnt ++;
	return EXIT_SUCCESS;
}

void USB_packetRxCallback(uint8_t *buf, uint32_t *len) {
	/* transmitting back the received message */
	// CDC_Transmit_FS(buf, *len);
	uint8_t chk = EXIT_FAILURE;
	bool time_out = false;

	switch (USB_rxConfig.state) {
	case idle:
		chk = setRxConfig(buf, *len);
		break;
	case receiving:
		if(isPacketTimedOut()){
			time_out = true;
			break;
		}
		chk = processNextPacket(buf, *len);
		if(chk == EXIT_FAILURE){
			break;
		}
		if(USB_rxConfig.num_packets == USB_rxConfig.packets_cnt){
			debug_log("Transmission completed, handling packet to application...");
			USB_rxConfig.application_cb(USB_rxConfig.buf, USB_rxConfig.buf_length);
			resetRxConfig();
		}
		break;
	}

	if(time_out){
		debug_log("ERROR: packet receive time exceeded!");
		debug_log("Trying to receive packet again as msg header...");
		USB_packetRxCallback(buf, len);
	}
}
