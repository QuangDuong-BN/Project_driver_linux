#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */

#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define BULK_EP_OUT 0x02
#define BULK_EP_IN 0x81

#define MAX_PKT_SIZE 512
#define VENDOR_SANDISK 0x0781
#define PRODUCT_USB30 0x5591


static struct usb_device *usb_flash;
static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_PKT_SIZE];

//dinh nghia cac ham thao tac
static int usb_flash_open(struct inode *i, struct file *f)
{
	return 0;
}

static int usb_flash_close(struct inode *i, struct file *f)
{
	return 0;
}

static ssize_t usb_flash_read(struct file *f, char __user *buf, size_t cnt, loff_t *off)
{
	int retval;
	int read_cnt;

	/* Doc du lieu tu usb flash */
	retval = usb_bulk_msg(usb_flash, usb_rcvbulkpipe(usb_flash, BULK_EP_IN),
	        bulk_buf, MAX_PKT_SIZE, &read_cnt, 5000);
	if (retval)
	{
		printk(KERN_ERR "read bulk message returned %d\n", retval);
		return retval;
	}
	if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt)))
	{
		return -EFAULT;
	}
	
	return MIN(cnt, read_cnt);
}

static ssize_t usb_flash_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
	int retval;
	int wrote_cnt = MIN(cnt, MAX_PKT_SIZE);

	if (copy_from_user(bulk_buf, buf, MIN(cnt, MAX_PKT_SIZE)))
	{
		return -EFAULT;
	}
	/* ghi du lieu vao usb flash */
	retval = usb_bulk_msg(usb_flash, usb_sndbulkpipe(usb_flash, BULK_EP_OUT),
		bulk_buf, MIN(cnt, MAX_PKT_SIZE), &wrote_cnt, 5000);
	if (retval)
	{
		printk(KERN_ERR "write bulk message returned %d\n", retval);
		return retval;
	}

	return wrote_cnt;
}

static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = usb_flash_open,
	.release = usb_flash_close,
	.read = usb_flash_read,
	.write = usb_flash_write,
};

static int usb_flash_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	int retval;
	
	usb_flash = interface_to_usbdev(interface);
	
	class.name = "usb/flash%d";
	class.fops = &fops;
	if ((retval = usb_register_dev(interface, &class)) < 0)
	{
		printk(KERN_ERR "Not able to get a minor for this device.");
	}
	else
	{
		printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
	}
	
	return retval;
}

static void usb_flash_disconnect(struct usb_interface *interface)
{
	usb_deregister_dev(interface, &class);
}

/* Bang cac thiet bi USB duoc dieu khien boi driver nay */
static struct usb_device_id usb_flash_table[] =
{
	{ USB_DEVICE(VENDOR_SANDISK, PRODUCT_USB30) },
	{}
};
MODULE_DEVICE_TABLE (usb, usb_flash_table);

static struct usb_driver usb_flash_driver =
{
	.name = "usb_flash_driver",
	.probe = usb_flash_probe,
	.disconnect = usb_flash_disconnect,
	.id_table = usb_flash_table,
};

static int __init usb_flash_init(void)
{
	int result;

	if ((result = usb_register(&usb_flash_driver)))
	{
		printk(KERN_ERR "usb_register failed. Error number %d", result);
	}
	return result;
}

static void __exit usb_flash_exit(void)
{
	usb_deregister(&usb_flash_driver);
}

module_init(usb_flash_init);
module_exit(usb_flash_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CNTT - KMA");
MODULE_DESCRIPTION("USB Flash Device Driver");
