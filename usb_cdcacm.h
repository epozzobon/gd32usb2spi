#ifndef __USB_CDCACM_H
#define __USB_CDCACM_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/gd32/gpio.h>
#include <libopencm3/usb/cdc.h>

void cdcacm_poll(void);
void cdcacm_init(void);
void cdcacm_line_state_changed_cb(uint8_t linemask);
void cdcacm_send_data(uint8_t *buf, uint16_t len);

#endif
