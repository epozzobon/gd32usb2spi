#pragma once

/* Pins for the GD32F150 MCU on the Tang Premier board */


/* User interaction pins
 *
 * We only have one red LED and one USER button.
 * Notice that the RESET button does not reset the GD32 MCU, only the FPGA.
 */
/* PA9 is connected to one of the red LEDs close to the USB port */
#define PORT_LED GPIOA
#define PIN_LED GPIO9
/* PA1 is the button marked "USER" on the PCB silk screen. */
#define PORT_USR_KEY GPIOA
#define PIN_USR_KEY GPIO1


/* USB pins
 *
 * Pull-up to D+ is provided by PB3, which allows a full hardware USB resets
 * to be performed.
 */
/* PB3 is pulling the USB+ line with a 1500 ohm resistor */
#define PORT_USB_PUL GPIOB
#define PIN_USB_PUL GPIO3
/* PA11 is USB- */
#define PORT_USBDM GPIOA
#define PIN_USBDM GPIO11
/* PA12 is USB+ */
#define PORT_USBDP GPIOA
#define PIN_USBDP GPIO12


/* FPGA programming pins
 *
 * These pins are connected directly to the FPGA JTAG interface, and can not
 * be mapped on the FPGA to do anything else.
 */
/* PA4 is connected to the FPGA JTAG TMS */
#define PORT_SPI1_NSS GPIOA
#define PIN_SPI1_NSS GPIO4
/* PA5 is connected to the FPGA JTAG TCK */
#define PORT_SPI1_SCK GPIOA
#define PIN_SPI1_SCK GPIO5
/* PA6 is connected to the FPGA JTAG TDO */
#define PORT_SPI1_MI GPIOA
#define PIN_SPI1_MI GPIO6
/* PA7 is connected to the FPGA JTAG TDI */
#define PORT_SPI1_MO GPIOA
#define PIN_SPI1_MO GPIO7


/* FPGA generic GPIO pins
 *
 * In total there are 8 GPIOs to be used for communicating to the FPGA.
 * These are connected to the pins for UART1, UART2 and SPI2 on the GD32.
 */
/* PB6 is connected to the FPGA pin J13 */
#define PORT_UART1_TX GPIOB
#define PIN_UART1_TX GPIO6
/* PB7 is connected to the FPGA pin H13 */
#define PORT_UART1_RX GPIOB
#define PIN_UART1_RX GPIO7
/* PA2 is connected to the FPGA pin L13 */
#define PORT_UART2_TX GPIOA
#define PIN_UART2_TX GPIO2
/* PA3 is connected to the FPGA pin M13 */
#define PORT_UART2_RX GPIOA
#define PIN_UART2_RX GPIO3
/* PA13 is connected to the FPGA pin L12 */
#define PORT_SPI2_MI GPIOA
#define PIN_SPI2_MI GPIO13
/* PA14 is connected to the FPGA pin J11 */
#define PORT_SPI2_MO GPIOA
#define PIN_SPI2_MO GPIO14
/* PA15 is connected to the FPGA pin K11 */
#define PORT_SPI2_NSS GPIOA
#define PIN_SPI2_NSS GPIO15
/* PB1 is connected to the FPGA pin P7 */
#define PORT_SPI2_SCK GPIOB
#define PIN_SPI2_SCK GPIO1

/* N.C. pins
 *
 * These pins are not connected anywhere and are overall useless.
 */
/* PA0 seems to be not connected  */
#define PORT_A0 GPIOA
#define PIN_A0 GPIO0
/* PB0 seems to be not connected  */
#define PORT_B0 GPIOB
#define PIN_B0 GPIO0
/* PA10 seems to be not connected  */
#define PORT_A10 GPIOA
#define PIN_A10 GPIO10
/* PB4 seems to be not connected  */
#define PORT_B4 GPIOB
#define PIN_B4 GPIO4
/* PB5 seems to be not connected  */
#define PORT_B5 GPIOB
#define PIN_B5 GPIO5
