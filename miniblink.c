/*
 * This file is part of the libopencm3 project.
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


#include "usb_driver.h"
#include "usb_cdcacm.h"

#include <stdlib.h>
#include <libopencm3/gd32/rcc.h>
#include <libopencm3/gd32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#define PORT_LED GPIOA
#define PIN_LED GPIO9

#define PORT_USB_PUL GPIOB
#define PIN_USB_PUL GPIO3


static void usb_task(void)
{
    rcc_periph_clock_enable(RCC_USB);
    gpio_set(PORT_USB_PUL, PIN_USB_PUL);

    cdcacm_init();
    
    while (1) {
        cdcacm_poll();
    }
}


static void gpio_setup(void)
{
    /* Enable GPIOA clock. */
    rcc_periph_clock_enable(RCC_GPIOA);

    /* Set GPIO9 (in GPIO port A) to 'output push-pull'. */
    gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);

    /* Set USB PULL UP */
    gpio_mode_setup(PORT_USB_PUL, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_USB_PUL);
}

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


int main(void)
{
    clock_setup_24mhz_xtal();
    gpio_setup();
    
    usb_task();

    /* Blink the LED (PA9) on the board. */
    while (1) {
        gpio_toggle(PORT_LED, PIN_LED);	/* LED on/off */

        uint32_t src = 72000000 / 4;

        __asm__ ("\n\t"
                "loop_here:         \n\t"
                /* One iteration takes exacly 4 clock cycles */
                "    sub %[c], #1   \n\t"
                "    cmp %[c], #0   \n\t"
                "    bne loop_here  \n\t"
                :  : [c] "r" (src));
    }

    return 0;
}
