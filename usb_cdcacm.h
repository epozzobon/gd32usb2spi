#ifndef __USB_CDCACM_H
#define __USB_CDCACM_H

#include <stdlib.h>
#include <libopencm3/cm3/common.h>
#include <libopencm3/gd32/gpio.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/usb/usbd.h>

void cdcacm_poll(void);
const usbd_device *cdcacm_init(const usbd_driver * const driver);
void cdcacm_line_state_changed_cb(uint8_t linemask);
void cdcacm_send_data(uint8_t *buf, uint16_t len);

extern void cdcacm_on_data_rx_cb(uint8_t *buf, uint16_t len);
extern void cdcacm_on_set_line_state_cb(bool dtr, bool rts);
extern int cdcacm_on_set_line_coding_cb(uint32_t baud, uint8_t databits,
			    enum usb_cdc_line_coding_bParityType cdc_parity,
			    enum usb_cdc_line_coding_bCharFormat cdc_stopbits);

#endif
