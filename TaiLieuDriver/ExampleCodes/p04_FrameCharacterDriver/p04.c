/*
 * ten file: p04.c //character driver frame
 * tac gia : kma
 * ngay tao: 2020/1/30
 * mo ta   : char driver cho thiet bi gia lap vchar_device
 *           vchar_device la mot thiet bi nam tren RAM
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class

static int Major;

#define DRIVER_AUTHOR "CNTT-KMA"
#define DRIVER_DESC   "Trinh dieu khien thiet bi ky tu don gian - scull"
#define DRIVER_VERSION "1.0"

/******************************** I. OS specific - lam viec voi nhan *******************************/
/* cac ham entry points */

/* ham khoi tao driver */
//static int __init p04_init(void)
//{
	/* cap phat device number */

	/* tao device file */

	/* cap phat bo nho cho cac cau truc du lieu cua driver va khoi tao */

	/* khoi tao thiet bi vat ly */

	/* dang ky cac entry point voi kernel */

	/* dang ky ham xu ly ngat */

	//printk("Initialize vchar driver successfully\n");
	//return 0;
//}

/* ham ket thuc driver */
//static void __exit p04_exit(void)
//{
	/* huy dang ky xu ly ngat */

	/* huy dang ky entry point voi kernel */

	/* giai phong thiet bi vat ly */

	/* giai phong bo nho da cap phat cau truc du lieu cua driver */

	/* xoa bo device file */

	/* giai phong device number */

	//printk("Exit vchar driver\n");
//}
//-----------------------------------------------------------------------------------
static int my_open(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: open()\n");
  return 0;
}

static int my_close(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: close()\n");
  return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
  printk(KERN_INFO "Driver: read()\n");
  return 168;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
  printk(KERN_INFO "Driver: write()\n");
  return 127;//len;
}

static struct file_operations fops =
{
  .owner = THIS_MODULE,
  .open = my_open,
  .release = my_close,
  .read = my_read,
  .write = my_write
};

static int __init p04_init(void) /* Constructor */
{
	//Đăng ký thiết bị kiểu character với major id hệ thống cung cấp

     Major=register_chrdev(0,"KMA.p04",&fops);   

     if(Major<0)
     {
           printk(KERN_ALERT "Dang ky that bai\n");
    }

     else{

           printk(KERN_ALERT "Dang ky thiet bi thanh cong voi MajorID=%d\n",Major);
     }

	return 0;
/*
  printk(KERN_INFO "Khoi tao driver thanh cong");
  if (alloc_chrdev_region(&first, 0, 1, "Shweta") < 0)
  {
    return -1;
  }

  if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
  {
    unregister_chrdev_region(first, 1);
    return -1;
  }

  if (device_create(cl, NULL, first, NULL, "mynull") == NULL)
  {
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
  }

   cdev_init(&c_dev, &pugs_fops);

  if (cdev_add(&c_dev, first, 1) == -1)
  {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    return -1;
  }

  return 0;
*/
}

static void __exit p04_exit(void) /* Destructor */
{
/*
  cdev_del(&c_dev);
  device_destroy(cl, first);
  class_destroy(cl);
  unregister_chrdev_region(first, 1);
*/

	unregister_chrdev(Major,"KTMT-Device");    

  printk(KERN_INFO "Ket thuc driver ok");
}

/*****************************************************************/

/****************************** device specific - lam viec voi device *****************************/
/* ham khoi tao thiet bi */

/* ham giai phong thiet bi */

/* ham doc tu cac thanh ghi du lieu cua thiet bi */

/* ham ghi vao cac thanh ghi du lieu cua thiet bi */

/* ham doc tu cac thanh ghi trang thai cua thiet bi */

/* ham ghi vao cac thanh ghi dieu khien cua thiet bi */

/* ham xu ly tin hieu ngat gui tu thiet bi */

/******************************* device specific - END *****************************/

module_init(p04_init);
module_exit(p04_exit);

MODULE_LICENSE("GPL"); /* giay phep su dung cua module */
MODULE_AUTHOR(DRIVER_AUTHOR); /* tac gia cua module */
MODULE_DESCRIPTION(DRIVER_DESC); /* mo ta chuc nang cua module */
MODULE_VERSION(DRIVER_VERSION); /* mo ta phien ban cuar module */
//MODULE_SUPPORTED_DEVICE("testdevice"); 
/* kieu device ma module ho tro */