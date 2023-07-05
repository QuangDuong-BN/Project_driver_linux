/*
 * Chuong trinh nay bieu dien PCI driver. Thiet bi PCI nay thuoc kieu character device.
 * 
 * Driver nay co the cung luc phuc vu 8 thiet bi PCI
 * 
 * Khong gian dia chi cua I/O port cua moi thiet bi PCI co dia chi bat dau la BAR2.
 * I/O port cua moi thiet bi PCI gom 3 thanh ghi
 * STATUS: addr = BAR2 + 0
 *
 * READ_DATA: addr = BAR2 + 1
 *	Thanh ghi read-only. Moi lan, driver doc 1 byte va ghi vao user buffer
 *
 * WRITE_DATA: addr = BAR2 + 2
 *	Thanh ghi write-only. Moi lan, driver nhan duoc du lieu tu user buffer,
 *	driver ghi tung byte vao thanh ghi nay
 * 
 */

#include<linux/module.h> /* tat ca cac module deu can thu vien nay */
#include<linux/init.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include<linux/fs.h> /* thu vien nay chua cac ham cap phat/giai phong device number */
#include<linux/device.h> /* thu vien nay chua cac ham phuc vu tao device file */
#include<linux/cdev.h> /* thu vien cho cau truc cdev */
#include<linux/slab.h> /* thu vien chua ham kmalloc */
#include<linux/uaccess.h> /* thu vien chua cac ham trao doi du lieu giua user va kernel */
#include <linux/pci.h> /* thu vien chua cac ham phuc vu giao tiep voi cac thiet bi PCI */

#define DRIVER_AUTHOR "CNTT - KMA"
#define DRIVER_DESC "Vi du ve driver cho PCI"
#define MAX_DEVICE		8
#define DEVICE_NAME 		"virtual_pci"
#define BAR_IO			2

/*
 * Bang nay chua cac thiet bi ma driver nay ho tro
 */
static struct pci_device_id pci_id_table[] = {
	{ PCI_DEVICE(0x0810, 0x1991), },
	{ 0, }
};

/*
 * Thong bao cho he thong biet driver nay ho tro cac thiet bi PCI trong danh sach pci_id_table
 */
MODULE_DEVICE_TABLE(pci, pci_id_table);

/*
 * Cau truc nay dung de lien ket pci_dev voi cdev 
 */
struct pci_cdev {
	int minor;
	struct pci_dev *pci_dev; //dai dien cho thiet bi co chuan ket noi PCI
	struct cdev *cdev; //dai dien cho dac tinh character
};

static struct pci_cdev pci_cdev[MAX_DEVICE];
static dev_t devno;
static int major;
static struct class * device_class;

/* khoi tao mang pci_cdev */
static void pci_cdev_init(struct pci_cdev pci_cdev[], int size, int first_minor)
{
	int i;

	for(i=0; i<size; i++) {
		pci_cdev[i].minor   = first_minor++;
		pci_cdev[i].pci_dev = NULL;
		pci_cdev[i].cdev    = NULL;
	}
}

/*
 * them mot thiet bi PCI vao danh sach cac thiet bi ma driver nay xu ly
 * tra ve -1 neu driver nay dang dieu khien MAX_DEVICES thiet bi
 * tra ve minor number neu nhu driver van con kha nang tiep nhan them thiet bi PCI nay
 */
static int pci_cdev_add(struct pci_cdev pci_cdev[], int size, struct pci_dev *pdev)
{
	int i, res = -1;

	for(i=0; i<size; i++) {
		if (pci_cdev[i].pci_dev == NULL) {
			pci_cdev[i].pci_dev = pdev;
			res = pci_cdev[i].minor;
			break;
		}
	}
	
	return res;
}

/*
 * dua mot thiet bi PCI ra khoi danh sach cac thiet bi ma driver nay xu ly
 */
static void pci_cdev_del(struct pci_cdev pci_cdev[], int size, struct pci_dev *pdev)
{
	int i;

	for(i=0; i<size; i++) {
		if (pci_cdev[i].pci_dev == pdev) {
			pci_cdev[i].pci_dev = NULL;
		}
	}
}

static struct pci_dev *pci_cdev_search_pci_dev(struct pci_cdev pci_cdev[], int size, int minor)
{
	int i;
	struct pci_dev *pdev = NULL;

	for(i=0; i<size; i++) {
		if (pci_cdev[i].minor == minor) {
			pdev = pci_cdev[i].pci_dev;
			break;
		}
	}

	return pdev;	
}

static struct cdev *pci_cdev_search_cdev(struct pci_cdev pci_cdev[], int size, int minor)
{
	int i;
	struct cdev *cdev = NULL;

	for(i=0; i<size; i++) {
		if (pci_cdev[i].minor == minor) {
			cdev = pci_cdev[i].cdev;
			break;
		}
	}

	return cdev;	
}

static int pci_cdev_search_minor(struct pci_cdev pci_cdev[], int size, struct pci_dev *pdev)
{
	int i, minor = -1;

	for(i=0; i<size; i++) {
		if (pci_cdev[i].pci_dev == pdev) {
			minor = pci_cdev[i].minor;
			break;
		}
	}

	return minor;
}



/*
 * Xac dinh thiet bi PCI nao can duoc phuc vu.
 *
 */
static int pci_open(struct inode *inode, struct file *file)
{
	int minor = iminor(inode);
	file->private_data = (void *)pci_cdev_search_pci_dev(pci_cdev, MAX_DEVICE, minor);
	return 0;
}

static int pci_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t pci_read(struct file *file, char *buffer, size_t length,	loff_t * offset)
{
	int byte_read = 0;
	unsigned char value;
	struct pci_dev *pdev = (struct pci_dev *)file->private_data;
	unsigned long pci_io_addr = 0;

	pci_io_addr = pci_resource_start(pdev, BAR_IO);

	while (byte_read < length) {
		/* doc 1 byte tu IO port */
		value = inb(pci_io_addr + 1);

		/* ghi gia tri vua doc vao user buffer */
		put_user(value, &buffer[byte_read]);

		byte_read++;
	}

	return byte_read;
}

static ssize_t pci_write(struct file *filp, const char *buffer, size_t len, loff_t * off) {
	int byte_write;
	unsigned char value;
	struct pci_dev *pdev = (struct pci_dev *)filp->private_data;
	unsigned long pci_io_addr = 0;

	pci_io_addr = pci_resource_start(pdev, BAR_IO);
	
	for(byte_write = 0; byte_write < len; byte_write++) {
		/* doc gia tri tu buffer */
		value = (unsigned char)buffer[byte_write];

		/* ghi gia tri vao thanh ghi cua thiet bi PCI */
		outb(pci_io_addr+2, value);
	}

	return byte_write;
}

static struct file_operations pci_ops = {
	.owner		= THIS_MODULE,
	.read 		= pci_read,
	.write 		= pci_write,
	.open 		= pci_open,
	.release 	= pci_release
};


/*
 * Khi mot thiet bi PCI cam vao he thong may tinh, ham probe cua driver se duoc goi
 *
 * return: 0 => driver khong dieu khien thiet bi PCI nay
 *         1 => driver chiu trach nhiem dieu khien thiet bi PCI nay
 *
 */
static int pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	int ret, minor;
	struct cdev *cdev;
	dev_t devno;

	printk("probe start\n");

	/* them thiet bi PCI moi nay vao trong danh sach pci_cdev */
	if ((minor = pci_cdev_add(pci_cdev, MAX_DEVICE, dev)) < 0)
		goto error;

	devno = MKDEV(major, minor);

	/* tao device file /dev/virtual_pci */
	device_class = class_create(THIS_MODULE, "class_char_pci_dev");
	device_create(device_class, NULL, devno, NULL, DEVICE_NAME);

	/* lien ket cac ham entry point cua driver voi device file */
	cdev = cdev_alloc();
	cdev_init(cdev, &pci_ops);
	ret = cdev_add(cdev, devno, 1);
	if (ret < 0) {
		dev_err(&(dev->dev), "Can't register character device\n");
		goto error;
	}
	/* lien ket cdev va pci_dev */
	pci_cdev[minor].cdev = cdev;

	/* bat thiet bi PCI */
	pci_enable_device(dev);

	/* cap phat khong gian dia chi cho I/O port de noi chuyen voi thiet bi PCI */
	if (pci_request_region(dev, BAR_IO, "IO-pci") == 0) {
		dev_err(&(dev->dev), "Can't request BAR2\n");
		cdev_del(cdev);
		goto error;
	}

	/* Kiem tra xem mien khong gian dia chi nay co phai la I/O port khong */
	if ((pci_resource_flags(dev, BAR_IO) & IORESOURCE_IO) != IORESOURCE_IO) {
		dev_err(&(dev->dev), "BAR2 isn't an IO region\n");
		cdev_del(cdev);
		goto error;
	}

	printk("probe done\n");

	return 1;

error:
	return 0;
}

/*
 * Ham nay duoc goi khi PCI driver bi unload khoi kernel hoac khi thiet bi PCI bi rut ra khoi he thon
 *
 */
static void pci_remove(struct pci_dev *dev)
{
	int minor;
	struct cdev *cdev;

	/* huy cdev lien ket voi pci dev */
	minor = pci_cdev_search_minor(pci_cdev, MAX_DEVICE, dev);
	cdev = pci_cdev_search_cdev(pci_cdev, MAX_DEVICE, minor);
	if (cdev != NULL) 
		cdev_del(cdev);

	/* huy pci dev khoi danh sach pci_cdev */
	pci_cdev_del(pci_cdev, MAX_DEVICE, dev);

	/* giai phong khong gian dia chi da cap phat cho I/O port cua thiet bi pci nay */
	pci_release_region(dev, BAR_IO);
}

/*
 * Cau truc nay giu cac thong tin cua pci driver
 *
 */
static struct pci_driver pci_driver = {
	.name 		= "ex_pci",
	.id_table 	= pci_id_table,
	.probe 		= pci_probe,
	.remove 	= pci_remove,
};


/*
 * Ham nay duoc goi khi module duoc load vao
 */
static int __init pci_init_module(void)
{
	int ret;

	printk("Module pci init\n");

	/* cap phat dong device number */
	ret = alloc_chrdev_region(&devno, 0, MAX_DEVICE, "buffer");

	/* luu lai major number */
	major = MAJOR(devno);

	/* khoi tao danh sach pci_cdev */
	pci_cdev_init(pci_cdev, MAX_DEVICE, MINOR(devno));

	/* dang ky driver nay voi kernel */
	ret = pci_register_driver(&pci_driver);
	if (ret < 0) {
		/* neu that bai, giai phong device number */
		unregister_chrdev_region(devno, 1);

		printk("pci-driver: can't register pci driver\n");
		return ret;
	}


	return 0;
}

/*
 * Ham nay duoc goi khi module bi unload khoi kernel
 */
static void __exit pci_exit_module(void)
{
	int i;

	/* huy dang ky pci driver */
	pci_unregister_driver(&pci_driver);

	/* huy dang ky character device */
	for(i=0; i< MAX_DEVICE; i++) {
		if (pci_cdev[i].pci_dev != NULL) {
			cdev_del(pci_cdev[i].cdev);
		}
	}


        device_destroy(device_class,devno);
        class_destroy(device_class);

	/* giai phong device number */
	unregister_chrdev_region(devno, MAX_DEVICE);
	printk("Remove character driver successfully.\n");
}

module_init(pci_init_module);
module_exit(pci_exit_module);


MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("2.4");
MODULE_SUPPORTED_DEVICE("testPCIdevice");
