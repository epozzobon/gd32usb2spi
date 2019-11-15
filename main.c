/*
 * Copyright (C) 2019 Enrico Pozzobon <enrico@epozzobon.it>
 *
 * Most of the code was taken from the libopencm3 project examples.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Copyright (C) 2012 Karl Palsson <karlp@tweak.net.au>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "usb_cdcacm.h"
#include "ringbuf.h"
#include "pin_assignments.h"

#include <stdlib.h>
#include <libopencm3/gd32/rcc.h>
#include <libopencm3/gd32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/gd32/f1x0/nvic.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/scb.h>


struct ringbuf rxring;
uint8_t rxbuf[128];


static void clock_setup_24mhz_xtal(void)
{
    struct rcc_clock_scale clock = {
        .pllmul = RCC_CFGR_PLLMUL_PLL_CLK_MUL3,  /* SYS at 3 times crystal: 72MHz */
        .hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,  /* AHB at same clock as SYS */
        .ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,  /* APB1 at same clock as AHB */
        .ppre2 = RCC_CFGR_PPRE2_HCLK_NODIV,  /* APB2 at same clock as AHB */
        .adcpre = RCC_CFGR_ADCPRE_PCLK2_DIV6,  /* ADC at one sixth of AHB, 12 MHz */
        .usbpre = RCC_CFGR_USBPRE_PLL_CLK_DIV1_5,  /* USB prescaler must be 1.5 to divide 72MHz to 48MHz */
        .use_hse = 1,  /* Use external high-speed clock source */
        .pll_hse_prediv = RCC_CFGR_PLLXTPRE_HSE_CLK,  /* Don't divide crystal (24MHz) */
        .ahb_frequency = 72000000,
        .apb1_frequency = 72000000,
        .apb2_frequency = 72000000
    };

    rcc_clock_setup_pll(&clock);
}


static void gpio_setup(void)
{
    /* Enable GPIOA clock. */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    /* Set GPIOs to 'output push-pull'. */
    gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);
    gpio_mode_setup(PORT_USB_PUL, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_USB_PUL);
    gpio_mode_setup(PORT_SPI2_NSS, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_SPI2_NSS);
    gpio_mode_setup(PORT_SPI2_SCK, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_SPI2_SCK);
    gpio_mode_setup(PORT_SPI2_MO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_SPI2_MO);

    /* Set GPIOs to 'floating input'. */
    gpio_mode_setup(PORT_USR_KEY, GPIO_MODE_INPUT, GPIO_PUPD_NONE, PIN_USR_KEY);
    gpio_mode_setup(PORT_SPI2_MI, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_SPI2_MI);
}


static void spi2_setup(void)
{
    rcc_periph_clock_enable(RCC_SPI2);
    spi_reset(SPI2);

    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
        SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

    spi_enable_software_slave_management(SPI2);
    spi_set_nss_high(SPI2);

    spi_enable(SPI2);
}


static void usb_setup(void)
{
    rcc_periph_clock_enable(RCC_USB);
    gpio_set(PORT_USB_PUL, PIN_USB_PUL);

    cdcacm_init(&st_usbfs_v1_usb_driver);
    
	nvic_enable_irq(NVIC_USB_LP_IRQ);
	nvic_enable_irq(NVIC_USB_HP_IRQ);
	nvic_enable_irq(NVIC_USB_WAKEUP_IRQ);
}


static void delay_ms(unsigned int ms)
{
    const unsigned int processor_clock = 72000000;
    const unsigned int clks_each_loop = 4;
    uint32_t counter = processor_clock / clks_each_loop / 1000 * ms;
    __asm__ ("\n\t"
        "loop_here:         \n\t"
        /* One iteration takes exacly clks_each_loop clock cycles */
        "    sub %[c], #1   \n\t"
        "    cmp %[c], #0   \n\t"
        "    bne loop_here  \n\t"
        :  : [c] "r" (counter));
}


int main(void)
{
    clock_setup_24mhz_xtal();
    cm_enable_interrupts();

    ringbuf_init(&rxring, rxbuf, sizeof(rxbuf));

    gpio_setup();
    spi2_setup();
    usb_setup();

    /* If this program is started with the key pressed,
     * then wait for it to be released */
    while (!gpio_get(PORT_USR_KEY, PIN_USR_KEY));
    
    /* The next time the key is pressed, go into SWD programming mode */
    while (gpio_get(PORT_USR_KEY, PIN_USR_KEY)) {
        size_t i;
        uint8_t txbuf[128];
        for (i = 0; i < 128 && !ringbuf_empty(&rxring); i++) {
            uint8_t r = ringbuf_read_byte(&rxring);
            txbuf[i] = r = (uint8_t) spi_xfer(SPI2, r);
        }
        if (i > 0) {
            cdcacm_send_data(txbuf, i);
        }
    }

    /* Enter SWD debugging mode, by assigning the SWD pins back to
     * their alternate function */

    gpio_mode_setup(PORT_SPI2_MI, GPIO_MODE_AF, GPIO_PUPD_NONE, PIN_SPI2_MI);
    gpio_mode_setup(PORT_SPI2_MO, GPIO_MODE_AF, GPIO_PUPD_NONE, PIN_SPI2_MO);

    /* wait for it to be released (delays for debouncing) */
    delay_ms(100);
    while (!gpio_get(PORT_USR_KEY, PIN_USR_KEY));
    delay_ms(100);

    /* Blink the LED until the key is pressed again */
    while (gpio_get(PORT_USR_KEY, PIN_USR_KEY)) {
        gpio_toggle(PORT_LED, PIN_LED);	/* LED on/off */
        delay_ms(125); /* 4 complete blinks per second */
    }

    /* If the button gets pressed again, perform a software reset */
    /* This is likely to lead back to the DFU bootloader */
    SCB_AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
    while(1);

    return 0;
}


void cdcacm_on_data_rx_cb(uint8_t *buf, uint16_t len)
{
    ringbuf_write(&rxring, buf, len);
}


void cdcacm_on_set_line_state_cb(bool dtr, bool rts)
{
    /* Ready To Send signal controls chip select */
    if (rts) {
        gpio_clear(PORT_SPI2_NSS, PIN_SPI2_NSS);
        gpio_set(PORT_LED, PIN_LED);
    } else {
        gpio_set(PORT_SPI2_NSS, PIN_SPI2_NSS);
        gpio_clear(PORT_LED, PIN_LED);
    }

    /* TODO: Think of something nice to use DTR for */
    if (dtr) {
    } else {
    }
}


int cdcacm_on_set_line_coding_cb(uint32_t baud, uint8_t databits,
			    enum usb_cdc_line_coding_bParityType cdc_parity,
			    enum usb_cdc_line_coding_bCharFormat cdc_stopbits)
{
    /* TODO: set SPI2 data rate usign baud */
    (void) baud;

    /* TODO: set SPI2 data bits using databits */
    (void) databits;

    switch (cdc_parity) {
        case USB_CDC_NO_PARITY:
        case USB_CDC_ODD_PARITY:
        case USB_CDC_EVEN_PARITY:
        case USB_CDC_MARK_PARITY:
        case USB_CDC_SPACE_PARITY:
            /* TODO: Think of something nice to use parity for */
            break;
    }
    switch (cdc_stopbits) {
        case USB_CDC_1_STOP_BITS:
        case USB_CDC_1_5_STOP_BITS:
        case USB_CDC_2_STOP_BITS:
            /* TODO: Think of something nice to use char format for */
            break;
    }
    return 1;
}

void usb_lp_isr(void)
{
    cdcacm_poll();
}
