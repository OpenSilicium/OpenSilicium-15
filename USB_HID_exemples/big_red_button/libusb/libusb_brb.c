/*
 * Panic button control program, libusb-0.1 based
 */
#include <stdio.h>
#include <stdlib.h>
#include <usb.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>

// Panic Button IDs
#define VENDOR_ID       0x1d34
#define PRODUCT_ID 	0x000d

// 8 bytes
#define MSG_SIZE        8

// Let's find the device
usb_dev_handle *find_usb_device() 
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for (bus = usb_busses; bus; bus = bus->next) {
      for (dev = bus->devices; dev; dev = dev->next) {
	if (dev->descriptor.idVendor == VENDOR_ID && dev->descriptor.idProduct == PRODUCT_ID) {
	  struct usb_dev_handle *handle;
	  printf ("USB Device with Vendor Id: %x and Product Id: %x found.\n", VENDOR_ID, PRODUCT_ID);
	  if (!(handle = usb_open (dev))) {
	    fprintf (stderr, "Could not open USB device\n");
	    return NULL;
	  }
	  
	  return handle;
	}
   	
      }
    }
	
    return NULL;
}


static  void detach_interface (usb_dev_handle *dev, int interface)
{
  int ret;

  /* try to detach device in case usbhid has got hold of it */
  ret = usb_detach_kernel_driver_np (dev, interface);
  
  if (ret != 0) {
    if (errno == ENODATA) {
      printf ("Detach interface %d already done\n", interface);
    } else {
      printf ("Detach interface %d failed: %s[%d]\n", interface, strerror (errno), errno);
    }
  } else {
    printf ("Detach interface %d successful\n", interface);
  }
}

static usb_dev_handle *setup_libusb_access () 
{
    usb_dev_handle *dev;

    usb_init ();
    usb_find_busses ();
    usb_find_devices ();
            
    if (!(dev = find_usb_device ())) {
      fprintf (stderr, "Couldn't find the device, Exiting\n");
      return NULL;
    }

    detach_interface (dev, 0);
	
    return dev;
}

int main ( int argc, char **argv)
{
  char* buffer;
  int n, i;
  usb_dev_handle *dev;


  buffer = (char*)malloc (MSG_SIZE);

  if (!buffer) {
    perror ("malloc");
    exit (1);
  }

  memset ((void*)buffer, 0, MSG_SIZE);
  
  if (!(dev = setup_libusb_access ()))
    exit(1);

  if (usb_claim_interface (dev, 0) < 0) {
    fprintf(stderr, "Could not claim interface 0: %s\n", usb_strerror());
  }

  while (1) {
    int l;

    memset ((void*)buffer, 0, MSG_SIZE);
    // demande état => 0x02
    buffer[7] = 0x02;

    if ((n = usb_control_msg (dev,  USB_TYPE_CLASS | USB_RECIP_INTERFACE |  USB_ENDPOINT_OUT /* 0x21 */, 0x09, 0x200, 0x00, buffer, 8,  0)) < 0)
      fprintf(stderr, "usb_interrupt_msg : %s (%d)\n", usb_strerror(), n);

    memset ((void*)buffer, 0, MSG_SIZE);
    if ((n = usb_interrupt_read (dev,  0x81 , buffer, 8, 1000)) < 0)
      fprintf(stderr, "usb_interrupt_read : %s (%d)\n", usb_strerror(), n);
    else 
      printf ("%02x\n", buffer[0]);
      
    usleep (200000);
  }

}
