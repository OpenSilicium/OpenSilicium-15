#include <linux/module.h>
#include <rtdm/rtdm_driver.h>
#include <linux/types.h>

#define RTDM_SUBCLASS_MYCLASS 0
#define DEVICE_NAME           "hello_rtdm"
#define BUF_SIZE              64

MODULE_DESCRIPTION("RTDM Hello World driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pierre Ficheux");

struct hello_context {
  int datasize;
  char buffer[BUF_SIZE];
};

int hello_open(struct rtdm_dev_context *context, rtdm_user_info_t *user_info, int oflags)
{
  struct hello_context *ctx;
  
  ctx = (struct hello_context *) context->dev_private;
  ctx->datasize = 0;

  rtdm_printk ("%s\n", __FUNCTION__);

  return 0;
}

int hello_close(struct rtdm_dev_context *context, rtdm_user_info_t *user_info)
{
  rtdm_printk ("%s\n", __FUNCTION__);

  return 0;
}

static ssize_t hello_write_rt(struct rtdm_dev_context *context, rtdm_user_info_t * user_info, const void *buf, size_t nbyte)
{
  int err;
  struct hello_context *ctx;
  ctx = (struct hello_context *) context->dev_private;

  ctx->datasize = (nbyte > BUF_SIZE) ? BUF_SIZE : nbyte;
  if ((err = rtdm_safe_copy_from_user (user_info, ctx->buffer, buf, ctx->datasize)) < 0) {
    rtdm_printk("%s: can't copy from user-space\n", __FUNCTION__);
    return err;
  }

  rtdm_printk ("%s: <%s>\n", __FUNCTION__, ctx->buffer);

  return ctx->datasize;
}

static struct rtdm_device device = {
 struct_version:         RTDM_DEVICE_STRUCT_VER,
 
 device_flags:           RTDM_NAMED_DEVICE,
 context_size:           sizeof(struct hello_context),
 device_name:            DEVICE_NAME,

 open_rt:                NULL,
 open_nrt:               hello_open,

 ops:{
  close_rt:       NULL,
  close_nrt:      hello_close,
  
  ioctl_rt:       NULL,
  ioctl_nrt:      NULL,
  
  read_rt:        NULL,
  read_nrt:       NULL,

  write_rt:       NULL,
  write_nrt:      hello_write_rt,
  },

 device_class:           RTDM_CLASS_EXPERIMENTAL,
 device_sub_class:       RTDM_SUBCLASS_MYCLASS,
 driver_name:            "hello_rtdm",
 driver_version:         RTDM_DRIVER_VER(0, 0, 0),
 peripheral_name:        "HELLO RTDM",
 provider_name:          "PF",
 proc_name:              device.device_name,
};

int __init hello_init(void)
{
  rtdm_printk("HELLO RTDM, loading\n");

  return rtdm_dev_register (&device); 
}

void __exit hello_exit(void)
{
  rtdm_printk("HELLO RTDM, unloading\n");

  rtdm_dev_unregister (&device, 1000);
}

module_init(hello_init);
module_exit(hello_exit); 

