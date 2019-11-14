#include <stdlib.h>
#include <libopencm3/usb/usbd.h>

#include "usb_driver.h"

#define USB_REGISTERS ((uint32_t *) 0x40005c00)
#define USB_REGISTERS_EPxCS (USB_REGISTERS)
#define USB_REGISTERS_EPxTBCNT (USB_REGISTERS + (0x20 >> 2))
#define USB_REGISTERS_CTL (USB_REGISTERS[0x40 >> 2])
#define USB_REGISTERS_INTF (USB_REGISTERS[0x44 >> 2])
#define USB_REGISTERS_STAT (USB_REGISTERS[0x48 >> 2])
#define USB_REGISTERS_DADDR (USB_REGISTERS[0x4c >> 2])
#define USB_REGISTERS_BADDR (USB_REGISTERS[0x50 >> 2])


uint8_t _gd_usb_device_allocation[256];

/* temporary references to ST library while I work on my own */
static usbd_device *   (*st_usb_driver_init             )(void                                                                                               );
static void            (*st_usb_driver_set_address      )(usbd_device *usbd_dev, uint8_t addr                                                                );
static void            (*st_usb_driver_ep_setup         )(usbd_device *usbd_dev, uint8_t addr, uint8_t type, uint16_t max_size, usbd_endpoint_callback cb    );
static void            (*st_usb_driver_ep_reset         )(usbd_device *usbd_dev                                                                              );
static void            (*st_usb_driver_ep_stall_set     )(usbd_device *usbd_dev, uint8_t addr, uint8_t stall                                                 );
static void            (*st_usb_driver_ep_nak_set       )(usbd_device *usbd_dev, uint8_t addr, uint8_t nak                                                   );
static uint8_t         (*st_usb_driver_ep_stall_get     )(usbd_device *usbd_dev, uint8_t addr                                                                );
static uint16_t        (*st_usb_driver_ep_write_packet  )(usbd_device *usbd_dev, uint8_t addr, const void *buf, uint16_t len                                 );
static uint16_t        (*st_usb_driver_ep_read_packet   )(usbd_device *usbd_dev, uint8_t addr, void *buf, uint16_t len                                       );
static void            (*st_usb_driver_poll             )(usbd_device *usbd_dev                                                                              );
static void            (*st_usb_driver_disconnect       )(usbd_device *usbd_dev, bool disconnected                                                           );
static void            (*st_usb_driver_44               )(void                                                                                               );
static void            (*st_usb_driver_48               )(void                                                                                               );


/* This is bad but I'm too lazy to define a structure */
const void * _gd_usb_driver_allocation[13] = {
    (const void*) gd_usb_driver_init,
    (const void*) gd_usb_driver_set_address,
    (const void*) gd_usb_driver_ep_setup,
    (const void*) gd_usb_driver_ep_reset,
    (const void*) gd_usb_driver_ep_stall_set,
    (const void*) gd_usb_driver_ep_nak_set,
    (const void*) gd_usb_driver_ep_stall_get,
    (const void*) gd_usb_driver_ep_write_packet,
    (const void*) gd_usb_driver_ep_read_packet,
    (const void*) gd_usb_driver_poll,           
    (const void*) 0,
    (const void*) 0,
    (const void*) 0 
};

usbd_device *gd_usb_driver_init(void)
{

    st_usb_driver_init             = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 0];
    st_usb_driver_set_address      = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 1];
    st_usb_driver_ep_setup         = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 2];
    st_usb_driver_ep_reset         = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 3];
    st_usb_driver_ep_stall_set     = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 4];
    st_usb_driver_ep_nak_set       = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 5];
    st_usb_driver_ep_stall_get     = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 6];
    st_usb_driver_ep_write_packet  = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 7];
    st_usb_driver_ep_read_packet   = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 8];
    st_usb_driver_poll             = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [ 9];
    st_usb_driver_disconnect       = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [10];
    st_usb_driver_44               = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [11];
    st_usb_driver_48               = (void *) ((const uint32_t *) &st_usbfs_v1_usb_driver) [12];

    _gd_usb_driver_allocation[ 0] = st_usb_driver_init            ;
    _gd_usb_driver_allocation[ 1] = st_usb_driver_set_address     ;
    _gd_usb_driver_allocation[ 2] = st_usb_driver_ep_setup        ;
    _gd_usb_driver_allocation[ 3] = st_usb_driver_ep_reset        ;
    _gd_usb_driver_allocation[ 4] = st_usb_driver_ep_stall_set    ;
    _gd_usb_driver_allocation[ 5] = st_usb_driver_ep_nak_set      ;
    _gd_usb_driver_allocation[ 6] = st_usb_driver_ep_stall_get    ;
    _gd_usb_driver_allocation[ 7] = st_usb_driver_ep_write_packet ;
    _gd_usb_driver_allocation[ 8] = st_usb_driver_ep_read_packet  ;
    _gd_usb_driver_allocation[ 9] = st_usb_driver_poll            ;
    _gd_usb_driver_allocation[10] = st_usb_driver_disconnect      ;
    _gd_usb_driver_allocation[11] = st_usb_driver_44              ;
    _gd_usb_driver_allocation[12] = st_usb_driver_48              ;

    return st_usb_driver_init();
}


void gd_usb_driver_set_address(usbd_device *usbd_dev, uint8_t addr)
{
    st_usb_driver_set_address(usbd_dev, addr);
}


void gd_usb_driver_ep_setup(usbd_device *usbd_dev, uint8_t addr, uint8_t type,
         uint16_t max_size, usbd_endpoint_callback cb)
{
    st_usb_driver_ep_setup(usbd_dev, addr, type, max_size, cb);
}


void gd_usb_driver_ep_reset(usbd_device *usbd_dev)
{
    st_usb_driver_ep_reset(usbd_dev);
}


void gd_usb_driver_ep_stall_set(usbd_device *usbd_dev, uint8_t addr,
             uint8_t stall)
{
    st_usb_driver_ep_stall_set(usbd_dev, addr, stall);
}


void gd_usb_driver_ep_nak_set(usbd_device *usbd_dev, uint8_t addr, uint8_t nak)
{
    st_usb_driver_ep_nak_set(usbd_dev, addr, nak);
}


uint8_t gd_usb_driver_ep_stall_get(usbd_device *usbd_dev, uint8_t addr)
{
    return st_usb_driver_ep_stall_get(usbd_dev, addr);
}


uint16_t gd_usb_driver_ep_write_packet(usbd_device *usbd_dev, uint8_t addr,
                const void *buf, uint16_t len)
{
    return st_usb_driver_ep_write_packet(usbd_dev, addr, buf, len);
}


uint16_t gd_usb_driver_ep_read_packet(usbd_device *usbd_dev, uint8_t addr,
               void *buf, uint16_t len)
{
    return st_usb_driver_ep_read_packet(usbd_dev, addr, buf, len);
}


void gd_usb_driver_poll(usbd_device *usbd_dev)
{
    st_usb_driver_poll(usbd_dev);
}


