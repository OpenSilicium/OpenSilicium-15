#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

// Panic Button IDs
#define VENDOR_ID       0x1d34
#define PRODUCT_ID 	0x000d

#define USB_STATUS_OK 0

libusb_device *device;
libusb_device_handle *handle;
struct libusb_config_descriptor *configure_descriptor;
int status;

main (int ac, char **av)
{
  char *buf = malloc (8);
  int err;

  /*==============================*/
  /*======== init libusb =========*/
  /*==============================*/
  status = libusb_init(NULL);
  if (status != USB_STATUS_OK) {
    fprintf (stderr, "libusb_init error, status= %d\n", status);
    exit (1);
  }


  /*=================================*/
  /*======== find/open USB device ===*/
  /*=================================*/
  handle  = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
  if (handle == NULL)
    {
      fprintf (stderr, "Could not find/open device\n");
      libusb_exit (NULL);
      exit (1);
    }
	
  device = libusb_get_device (handle);	
  if(device == NULL)	
    {
      fprintf(stderr, "Could not get device\n");
      libusb_close(handle);
      libusb_exit(NULL);
      exit (1);		
    }
	
  /*=========================================*/
  /*======== Get the configure desc =========*/
  /*=========================================*/
  status = libusb_get_active_config_descriptor(device, &configure_descriptor);
  if (status != USB_STATUS_OK) 
    {
      fprintf (stderr, "Failed to get configure descriptor, status= %d\n", status);
      libusb_close(handle);
      libusb_exit(NULL);
      exit (1);
    }
	
  /*==========================================================================*/
  /*======== if Interface is active on the kernel ==> Detach this interface ==*/
  /*==========================================================================*/			
  if (libusb_kernel_driver_active(handle, configure_descriptor->interface[0].altsetting[0].bInterfaceNumber) == 1) 
    {
      status = libusb_detach_kernel_driver(handle, configure_descriptor->interface[0].altsetting[0].bInterfaceNumber);
      if (status != USB_STATUS_OK) 
	{
	  fprintf(stderr, "Failed to detach kernel driver, status= %d\n", status);
	  libusb_free_config_descriptor(configure_descriptor);
	  libusb_close(handle);
	  libusb_exit(NULL);
	  return 1;
	}
    }

  /*=================================================*/
  /*======== Interface claim before any transfert  ==*/
  /*=================================================*/
  status = libusb_claim_interface(handle, configure_descriptor->interface[0].altsetting[0].bInterfaceNumber);	
  if(status != USB_STATUS_OK)
    {
      printf("error interface claim, error= %d\n", status);
      libusb_free_config_descriptor(configure_descriptor);
      libusb_close(handle);
      libusb_exit(NULL);
      exit (1);
    }

  //  memset ((void*)buf, 0, 8);
  //  libusb_control_transfer (handle,  0x21, 0x0a, 0x00, 0x00, buf, 8,  0);

  while (1) {
    int l, r;

    memset ((void*)buf, 0, 8);
    // demande état => 0x02
    buf[7] = 0x02;
    r = libusb_control_transfer (handle,  0x21, 0x09, 0x200, 0x00, buf, 8,  0);

    if (r < 0)
      printf ("control -> %d %s\n", r, libusb_error_name(r));

    memset ((void*)buf, 0, 8);
    r = libusb_interrupt_transfer (handle, 0x81, buf, 8, &l, 3000);

    if (r < 0)
      printf ("interrupt -> %d %d %s\n", r, l, libusb_error_name(r));
    else
      printf ("%02x\n", buf[0]);
	
    usleep (200000);
  }
}
