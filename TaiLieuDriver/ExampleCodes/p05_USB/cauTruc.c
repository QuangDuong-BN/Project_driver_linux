{

    …

    struct usb_device_descriptor descriptor;

    struct usb_host_config *config, *actconfig;

    …

};

struct usb_host_config

{

    struct usb_config_descriptor desc;

    …

    struct usb_interface *interface[USB_MAXINTERFACES];

    …

};

struct usb_interface

{

    struct usb_host_interface *altsetting /* array */, *cur_altsetting;

    …

};

struct usb_host_interface

{

    struct usb_interface_descriptor desc;

    struct usb_host_endpoint *endpoint /* array */;

    …

};

struct usb_host_endpoint

{

    struct usb_endpoint_descriptor desc;

    …

};