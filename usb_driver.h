#include <libopencm3/usb/usbd.h>

#define gd_usb_driver ((usbd_driver *) &_gd_usb_driver_allocation[0])
#define gd_usb_device ((usbd_device *) &_gd_usb_device_allocation[0])
extern const void * _gd_usb_driver_allocation[13];

usbd_device *gd_usb_driver_init             (void                                                                                               );
void         gd_usb_driver_set_address      (usbd_device *usbd_dev, uint8_t addr                                                                );
void         gd_usb_driver_ep_setup         (usbd_device *usbd_dev, uint8_t addr, uint8_t type, uint16_t max_size, usbd_endpoint_callback cb    );
void         gd_usb_driver_ep_reset         (usbd_device *usbd_dev                                                                              );
void         gd_usb_driver_ep_stall_set     (usbd_device *usbd_dev, uint8_t addr, uint8_t stall                                                 );
void         gd_usb_driver_ep_nak_set       (usbd_device *usbd_dev, uint8_t addr, uint8_t nak                                                   );
uint8_t      gd_usb_driver_ep_stall_get     (usbd_device *usbd_dev, uint8_t addr                                                                );
uint16_t     gd_usb_driver_ep_write_packet  (usbd_device *usbd_dev, uint8_t addr, const void *buf, uint16_t len                                 );
uint16_t     gd_usb_driver_ep_read_packet   (usbd_device *usbd_dev, uint8_t addr, void *buf, uint16_t len                                       );
void         gd_usb_driver_poll             (usbd_device *usbd_dev                                                                              );
void         gd_usb_driver_disconnect       (usbd_device *usbd_dev, bool disconnected                                                           );
