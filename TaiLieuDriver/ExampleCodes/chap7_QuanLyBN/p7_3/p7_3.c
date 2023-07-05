/*
 * char_driver_4.c - character driver hoan chinh
 */
#include <linux/module.h> /* tat ca cac module deu can thu vien nay */
#include <linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */
#include<linux/device.h> /* thu vien nay chua cac ham phuc vu tao device file */
#include<linux/cdev.h> /* thu vien cho cau truc cdev */
#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */

#define DRIVER_AUTHOR "CNTT - KMA"
#define DRIVER_DESC   "A simple example about character driver"
#define OBJ_SIZE 1024
#define MAX_MSG 128
#define EMPTY 0
#define FULL 1
#define NORMAL 2

dev_t dev_num = 0;
static struct class * device_class;
static struct cdev *example_cdev;
uint8_t *kernel_buffers[MAX_MSG]; //mang cac con tro. moi con tro se tro toi object duoc cap phat boi kmem_cache_alloc
int status_buffers = EMPTY; //bien luu trang thai cua mang kernel_buffers
int msgw_index = 0; //ta se ghi du lieu thong qua con tro thu msgw_index cua mang kernel_buffes
int msgr_index = 0; //ta se doc du lieu thong qua con tro thu msgr_index cua mang kernel_buffers
struct kmem_cache *buffer_cachep; //con tro, tro toi lookaside cache
unsigned open_cnt = 0;

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
	if (status_buffers == EMPTY)
	{
		printk("buffers is empty\n");
		return 0;
	}
	if (status_buffers == FULL) status_buffers = NORMAL;

	copy_to_user(user_buf, kernel_buffers[msgr_index], OBJ_SIZE);
	kmem_cache_free(buffer_cachep, kernel_buffers[msgr_index]);//sau khi doc xong, tra object lai cho lookaside cache

	msgr_index++;
	if(msgr_index == MAX_MSG) msgr_index = 0;
	if(msgr_index == msgw_index) status_buffers = EMPTY;

	printk("Handle read event %u times\n", open_cnt);
	return OBJ_SIZE;
}
static ssize_t example_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	if (status_buffers == FULL)
	{
		printk("buffers is full\n");
		return 0;
	}
	if (status_buffers == EMPTY) status_buffers = NORMAL;

	kernel_buffers[msgw_index] = kmem_cache_alloc(buffer_cachep, GFP_KERNEL);//truoc khi ghi, yeu cau lookaside cache cap phat 1 object
	copy_from_user(kernel_buffers[msgw_index], user_buf, len);

	msgw_index++;
	if(msgw_index == MAX_MSG) msgw_index = 0;
	if(msgw_index == msgr_index) status_buffers = FULL;

	printk("Handle write event %u times\n", open_cnt);
	return len;
}
 

static int __init char_driver_init(void)
{
	/* cap phat dong device number */
	alloc_chrdev_region(&dev_num, 0, 1, "char_dev");
	printk("Insert character driver successfully. major(%d), minor(%d)\n", MAJOR(dev_num), MINOR(dev_num));

	/* tao device file /dev/char_device */
	device_class = class_create(THIS_MODULE, "class_char_dev");
	device_create(device_class, NULL, dev_num, NULL,"char_device");

	/* tao lookaside cache co ten la buffer_cache. moi object la 1 buffer co kich thuoc OBJ_SIZE */
	buffer_cachep = kmem_cache_create("buffer_cache", OBJ_SIZE, 0, SLAB_PANIC, NULL);

	/* lien ket cac ham entry point cua driver voi device file */
	example_cdev = cdev_alloc();
	cdev_init(example_cdev, &fops);
	cdev_add(example_cdev, dev_num, 1);

	return 0;
}

void __exit char_driver_exit(void)
{
	cdev_del(example_cdev);
	kmem_cache_destroy(buffer_cachep);//huy bo lookaside cache
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

