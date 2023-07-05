/*
 * Su dung timer trong kernel
 */
#include <linux/module.h> /* tat ca cac module deu can thu vien nay */
#include <linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */
#include<linux/device.h> /* thu vien nay chua cac ham phuc vu tao device file */
#include<linux/cdev.h> /* thu vien cho cau truc cdev */
#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */
#include<linux/timer.h> /* thu vien chua cac ham thao tac voi kernel timer */

#define DRIVER_AUTHOR "CNTT - KMA"
#define DRIVER_DESC   "Tich hop timer vao character driver"
#define MEM_SIZE 1024

typedef struct operator {
	int a;
	int b;
} operator_t;

dev_t dev_num = 0;
static struct class * device_class;
static struct cdev *example_cdev;
uint8_t *kernel_buffer;
unsigned open_cnt = 0;
/* khai bao timer */
struct timer_list funny_timer;
operator_t funny_data;

static int example_open(struct inode *inode, struct file *filp);
static int example_release(struct inode *inode, struct file *filp);
static ssize_t example_read(struct file *filp, char __user *user_buf, size_t len, loff_t * off);
static ssize_t example_write(struct file *filp, const char *user_buf, size_t len, loff_t * off);
 
static struct file_operations fops =
{
	.owner          = THIS_MODULE,
	.read           = example_read,
	.write          = example_write,
	.open           = example_open,
	.release        = example_release,
};
 
static int example_open(struct inode *inode, struct file *filp)
{
	open_cnt++;
	printk("Handle opened event %u times\n", open_cnt);
	return 0;
}

static int example_release(struct inode *inode, struct file *filp)
{
        printk("Handle closed event %u times\n", open_cnt);
        return 0;
}
 
static ssize_t example_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	copy_to_user(user_buf, kernel_buffer, MEM_SIZE);
	printk("Handle read event %u times\n", open_cnt);
	return MEM_SIZE;
}
static ssize_t example_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	copy_from_user(kernel_buffer, user_buf, len);
	printk("Handle write event %u times\n", open_cnt);
	return len;
}

static void funny_function(struct timer_list * data);//(unsigned long data)
{
	operator_t *pdata = (operator_t*)data;
	if(pdata)
		printk("function for kernel timer: %d + %d = %d\n", pdata->a, pdata->b, pdata->a + pdata->b);
}

static int __init char_driver_init(void)
{
	/* cap phat dong device number */
	alloc_chrdev_region(&dev_num, 0, 1, "tenDevice_4.3");
	printk("Insert character driver successfully. major(%d), minor(%d)\n", MAJOR(dev_num), MINOR(dev_num));

	/* tao device file /dev/char_device */
	device_class = class_create(THIS_MODULE, "tenLopFileDevice_4.3");
	device_create(device_class, NULL, dev_num, NULL,"tenDeviceFile_4.3");

	/* tao kernel buffer */
	kernel_buffer = kmalloc(MEM_SIZE , GFP_KERNEL);

	/* lien ket cac ham entry point cua driver voi device file */
	example_cdev = cdev_alloc();
	cdev_init(example_cdev, &fops);
	cdev_add(example_cdev, dev_num, 1);

	/* khoi tao timer */
	funny_data.a = 20;
	funny_data.b = 10;
	init_timer(&funny_timer);
	funny_timer.expires = jiffies + (10 * HZ);
	funny_timer.function = funny_function;
	//funny_timer.data = (unsigned long)&funny_data;
	/* kich hoat timer */
	add_timer(&funny_timer);

	return 0;
}

void __exit char_driver_exit(void)
{
	if(del_timer(&funny_timer))
		printk("funny timer is active. Deactive it now!!!\n");

	cdev_del(example_cdev);
	kfree(kernel_buffer);
	device_destroy(device_class,dev_num);
	class_destroy(device_class);
	unregister_chrdev_region(dev_num, 1);
	printk("Remove character driver successfully.\n");
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("2.4");
MODULE_SUPPORTED_DEVICE("testdevice");

