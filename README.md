# gd32usb2spi
Simple USB-to-SPI(master) adapter firmware using the CDCACM driver.
Designed to be used on the Sipeed Tang Premier FPGA board, to establish
communication between the GD32F150 on-board processor and the Anlogic FPGA.

It is recommended to back up the original firmware of the GD32F150 before
installing this one.

After installing this firmware with `make flash`, the GD32F150 on the Tang
Premier will contain the following software:

| Address   | Content      | Notes                                 |
|-----------|--------------|---------------------------------------|
| 0x8000000 | dapboot      | Used to flash the GD32 MCU over DFU   |
| 0x8002000 | USB-to-JTAG  | Used to upload bitstreams to the FPGA |
| 0x8004000 | USB-to-SPI   | this repo                             |

The dapboot bootloader (at https://github.com/Lichee-Pi/dapboot) is used to 
flash other applications on the MCU, like the one in this repository.
If the USER key is pressed when the board is powered, dapboot will start in 
DFU mode, allowing you to flash anything at address 0x8002000.
Otherwise, if the button is not pressed, it will boot whatever is flashed at
address 0x8002000.

The Makefile in this repository will install a patched version of the 
USB-to-JTAG application, which will allow you to boot the application at
0x8004000 by pressing the USER key during operation.


# Installation

- Run `make` to build the firmware.
- Hold the "USER" key pressed on the Tang Premier board and connect it to your
  PC over USB. It should get pop up as device "1209:db42".
- Run `make flash` to flash the patched USB-to-JTAG firmware as well as this
  USB-to-SPI.
- The device will now reconnect as device "0547:1002" (USB-to-JTAG).
- Press the "USER" key again to start the USB-to-SPI firmware


# Usage

- Connect the Tang Premier board to USB without holding the "USER" key pressed
- Once the USB-to-JTAG device shows up, press the "USER" key to switch to
  USB-to-SPI mode

This USB-to-SPI firmware shows up as a CDC ACM serial device. The RTS signal
controls the select line of SPI. Each byte sent to the ACM device will be sent
over the MOSI line, and the response from MISO will appear on the ACM device.

