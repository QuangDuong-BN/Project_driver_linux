#include <linux/module.h> /* tat ca cac module deu can thu vien nay */
#include <linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */
#include<linux/device.h> /* thu vien nay chua cac ham phuc vu tao device file */
#include<linux/cdev.h> /* thu vien cho cau truc cdev */
#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */

#include<linux/time.h> /* thu vien chua cac ham thao tac voi wall time */
#include<linux/jiffies.h> /* thu vien chua ham thao tac voi system uptime */

//#include <linux/ktime.h>

#define DRIVER_AUTHOR "CNTT-KMA"
#define DRIVER_DESC   "A simple example about character driver"
#define MEM_SIZE 1024

dev_t dev_num = 0;
static struct class * device_class;
static struct cdev *example_cdev;
uint8_t *kernel_buffer;
unsigned open_cnt = 0;
unsigned long js, je;

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
	js = jiffies;//lay so tick hien tai trong bien cua OS la jiffies
	open_cnt++;
	printk("Mo file %u lan\n", open_cnt);
	return 0;
}

static int example_release(struct inode *inode, struct file *filp)
{
	struct timeval tv;
	printk("Giai phong %u times\n", open_cnt);
	je = jiffies;//lay so tick hien tai - thoi diem dong file
	jiffies_to_timeval(je-js, &tv); //tru de tinhs thoi gian

	//lay giay: tv_sec; micro giay: tv_usec
	printk("Driver duoc su dung luc %ld.%ld s\n", tv.tv_sec, tv.tv_usec/1000);
	return 0;
}
 
static ssize_t example_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	//lay time
	struct timespec64 kts;
	ktime_get_coarse_real_ts64(&kts);// = current_kernel_time64();//current_kernel_time();
	

	copy_to_user(user_buf, kernel_buffer, MEM_SIZE);
	printk("Doc file %u lan vao thoi diem %ld.%ld from 1 Jan 1970\n", open_cnt, kts.tv_sec, kts.tv_nsec/1000000);
	return MEM_SIZE;
}
static ssize_t example_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	//struct timeval ktv;
	struct timespec64 ktv;
	ktime_get_real_ts64(&ktv);//do_gettimeofday(&ktv);


	copy_from_user(kernel_buffer, user_buf, len);
	//printk("Handle write event %u times at %ld.%ld from 1 Jan 1970\n", open_cnt, ktv.tv_sec, ktv.tv_usec/1000);
	printk("Ghi file %u lan, vao thoi diem %ld.%ld from 1 Jan 1970\n", open_cnt, ktv.tv_sec, ktv.tv_sec/1000);
	return len;
}
 

static int __init char_driver_init(void)
{
	/* cap phat dong device number */
	alloc_chrdev_region(&dev_num, 0, 1, "device4.1");
	printk("Insert character driver successfully. major(%d), minor(%d)\n", MAJOR(dev_num), MINOR(dev_num));

	/* tao device file /dev/char_device */
	device_class = class_create(THIS_MODULE, "classDevieFile");
	device_create(device_class, NULL, dev_num, NULL,"deviceFile4_1");

	/* tao kernel buffer */
	kernel_buffer = kmalloc(MEM_SIZE , GFP_KERNEL);

	/* lien ket cac ham entry point cua driver voi device file */
	example_cdev = cdev_alloc();
	cdev_init(example_cdev, &fops);
	cdev_add(example_cdev, dev_num, 1);

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
MODULE_VERSION("2.4");
MODULE_SUPPORTED_DEVICE("testdevice");

