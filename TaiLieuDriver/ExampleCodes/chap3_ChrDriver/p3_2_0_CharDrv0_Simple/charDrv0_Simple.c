#include <linux/module.h> /* tat ca cac module deu can thu vien nay */
#include <linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */
#include<linux/device.h> /* thu vien nay chua cac ham phuc vu tao device file */
#include<linux/cdev.h> /* thu vien cho cau truc cdev */
#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */

#define DRIVER_AUTHOR "CNTT - KMA"
#define DRIVER_DESC   "Character driver don gian, cap phat dong, day du cac buoc"
#define MEM_SIZE 1024

dev_t dev_num = 0;
static struct class * device_class;
static struct cdev *example_cdev;
uint8_t *kernel_buffer;
unsigned open_cnt = 0;

//function prototype cho entry point
static int example_open(struct inode *inode, struct file *filp);
static int example_release(struct inode *inode, struct file *filp);
static ssize_t example_read(struct file *filp, char __user *user_buf, size_t len, loff_t * off);
static ssize_t example_write(struct file *filp, const char *user_buf, size_t len, loff_t * off);

//dang ky Entry point voi file_operations 
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
 

//ham khoi tao driver. Dung de dang ky voi kernel theo ham module_init
static int __init char_driver_init(void)
{
	/*1 cap phat dong device number, va gan voi ten device*/
	alloc_chrdev_region(&dev_num, 0, 1, "tenDevice3.0");
	printk("Insert character driver successfully. major(%d), minor(%d)\n", MAJOR(dev_num), MINOR(dev_num));

	/*2 tao device file /dev/tenDeviceFile */
	device_class = class_create(THIS_MODULE, "tenLop");

	device_create(device_class, NULL, dev_num, NULL,"tenDeviceFile");

	/* tao kernel buffer */
	kernel_buffer = kmalloc(MEM_SIZE , GFP_KERNEL);

	/*3 lien ket cac ham entry point cua driver voi device file */
	example_cdev = cdev_alloc();//cap phat bo nho cho bien cau truc cdev
	cdev_init(example_cdev, &fops); //khoi tao cho bien cau truc cdev
	cdev_add(example_cdev, dev_num, 1); //dang ky cau truc cdev voi kernel

	return 0;
}

void __exit char_driver_exit(void)
{
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
MODULE_VERSION("3.0");

