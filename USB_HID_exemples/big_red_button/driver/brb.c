/*
 * Linux driver for Big Red Button by Dream Cheeky
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <asm/uaccess.h>

#include "brb.h"

MODULE_AUTHOR("Pierre Ficheux, pierre.ficheux@gmail.com");
MODULE_DESCRIPTION("Big Red Button driver");
MODULE_LICENSE("GPL");

#define VENDOR_ID	0x1d34
#define PRODUCT_ID	0x000d

// Private structure
struct usb_brb {
	struct usb_device *	udev;
	unsigned int		status;
};

// Forward declaration 
static struct usb_driver brb_driver;

/* Table of devices that work with this driver */
static struct usb_device_id id_table [] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{ },
};
MODULE_DEVICE_TABLE (usb, id_table);

/* Ask BRB for status */
static int get_big_red_button_status (struct usb_brb *brb_dev)
{
  char buf[8];
  int ret = 0, l;

  memset (buf, 0, 8);
  buf[7] = 0x02;

  ret = usb_control_msg(brb_dev->udev, usb_sndctrlpipe(brb_dev->udev, 0), 0x09, USB_TYPE_CLASS | USB_RECIP_INTERFACE, 0x200, 0x00, buf, 8, 2 * HZ);

  if (ret < 0) {
    printk (KERN_WARNING "%s: IN, ret = %d\n", __FUNCTION__, ret);
    return ret;
  }

  memset (buf, 0, 8);
  ret = usb_interrupt_msg (brb_dev->udev, usb_rcvintpipe (brb_dev->udev, 1), buf, 8, &l, 2 * HZ);
  if (ret < 0) {
    printk (KERN_WARNING "%s: usb_interrupt_msg(), ret = %d l= %d\n", __FUNCTION__, ret, l);
    return ret;
  }
  else
    brb_dev->status = buf[0];
  
  return 0;
}

/*
 * /sys functions
 */
static ssize_t show_status(struct device *dev, struct device_attribute *attr, char *buf)
{
  struct usb_interface *intf = to_usb_interface(dev); 
  struct usb_brb *brb_dev = usb_get_intfdata(intf);

  get_big_red_button_status (brb_dev);

  return sprintf (buf, "%x\n", brb_dev->status); 
}


static ssize_t set_status(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	return count;
}
static DEVICE_ATTR(status, S_IWUGO | S_IRUGO, show_status, set_status);


// Char device functions

static int brb_open (struct inode *inode, struct file *file)
{
  struct usb_panicb *dev;
  struct usb_interface *interface;
  int minor;
  
  minor = iminor(inode);

  // Get interface for device
  interface = usb_find_interface (&brb_driver, minor);
  if (!interface)
    return -ENODEV;

  // Get private data from interface
  dev = usb_get_intfdata (interface);
  if (dev == NULL) {
    printk (KERN_WARNING "%s: can't find device for minor %d\n", __FUNCTION__, minor);
    return -ENODEV;
  }

  // Set to file structure
  file->private_data = (void *)dev;

  return 0;
}

static int brb_release (struct inode *inode, struct file *file)
{
  return 0;
}

static long brb_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  struct usb_brb *dev;

  /* get the dev object */
  dev = file->private_data;
  if (dev == NULL)
    return -ENODEV;

  switch (cmd) {
    case BRB_STATUS :
      printk(KERN_INFO "brb_ioctl\n");
      if (get_big_red_button_status (dev) == 0) {
	if (copy_to_user((void*)arg, &(dev->status), sizeof(int))) {
	  printk (KERN_WARNING "%s: copy_to_user() error\n", __FUNCTION__);
	  return -EFAULT;
	}
      }

      break;

    default :
      printk(KERN_WARNING "%s: unsupported command %d\n", __FUNCTION__, cmd);
      
      return -EINVAL;
  }

  return 0;
}

static struct file_operations brb_fops = {
  .open    = brb_open,
  .release = brb_release,
  .unlocked_ioctl   = brb_ioctl
};

// USB driver functions

static struct usb_class_driver brb_class_driver = {
  .name = "usb/brb%d",
  .fops = &brb_fops,
  .minor_base = 0
};

static int brb_probe (struct usb_interface *interface, const struct usb_device_id *id)
{
  struct usb_device *udev = interface_to_usbdev (interface);
  struct usb_brb *brb_dev;
  int ret;

  ret = usb_register_dev(interface, &brb_class_driver);
  if (ret < 0) {
    printk (KERN_WARNING "brb: usb_register_dev() error\n");
    return ret;
  }

  brb_dev = kmalloc (sizeof(struct usb_brb), GFP_KERNEL);
  if (brb_dev == NULL) {
    printk (KERN_WARNING "%s: Out of memory\n", __FUNCTION__);
    return -ENOMEM;
  }

  // Fill private structure and save it with usb_set_intfdata
  memset (brb_dev, 0x00, sizeof (*brb_dev));
  brb_dev->udev = usb_get_dev(udev);
  brb_dev->status = 0;
  usb_set_intfdata (interface, brb_dev);

  // Add /sys entry
  ret = device_create_file(&interface->dev, &dev_attr_status);
  if (ret < 0) {
    printk (KERN_WARNING "%s: device_create_file() error\n", __FUNCTION__);
    return ret;
  }

  printk (KERN_INFO "%s: USB Big Red Button device now attached\n", __FUNCTION__);

  return 0;
}

static void brb_disconnect(struct usb_interface *interface)
{
  struct usb_brb *dev;

  dev = usb_get_intfdata (interface);
  usb_deregister_dev (interface, &brb_class_driver);
  usb_set_intfdata (interface, NULL);

  device_remove_file(&interface->dev, &dev_attr_status);
  usb_put_dev(dev->udev);

  kfree(dev);

 printk (KERN_INFO "%s: USB Big Red Button device now disconnected\n", __FUNCTION__);
}

static struct usb_driver brb_driver = {
	.name       =	"brb",
	.probe      =	brb_probe,
	.disconnect =	brb_disconnect,
	.id_table   =	id_table,
};

// Init & exit

static int __init usb_brb_init(void)
{
  int retval = 0;

  retval = usb_register(&brb_driver);
  if (retval)
    printk(KERN_WARNING "%s: usb_register failed. Error number %d", __FUNCTION__, retval);

  return retval;
}

static void __exit usb_brb_exit(void)
{
  usb_deregister(&brb_driver);
}

module_init (usb_brb_init);
module_exit (usb_brb_exit);
