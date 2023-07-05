#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>

//#include <linux/bio.h>
//#include <linux/buffer_head.h>

#include "vblock_dev.h"

#define VBLOCK_FIRST_MINOR 0
#define VBLOCK_MINOR_COUNT 2
#define VBLOCK_DEVICE_NAME "vblock"

static struct vblock_device {
	int nsectors; //so sector cua vblock device
	u_int major; //so major number cua vblock device
	spinlock_t lock; //spinlock dung de bao ve request queue
	struct request_queue *queue; //hang doi chua cac request
	u8 *data; //noi chua du lieu
	struct gendisk *vb_disk; //cau truc gendisk de mo ta vblock device
}vblock_dev;

static void vb_handle_request(struct request_queue *q)
{
	int res = 0;
	int direction;
	char *buffer;
	unsigned int num_sectors, begin_sector;
	struct request *rq = blk_get_request(q, res, __REQ_NOWAIT);// blk_fetch_request(q); //lay ra yeu cau dau tien trong hang doi
	while(rq) {
		//kiem tra yeu cau nay co phai la mot yeu cau trao doi du lieu thong thuong khong
		/*
		if(rq->cmd_type != REQ_TYPE_FS) {
			printk("This is not a normal fs request, skipping\n");
			goto done;
		}
		*/
		//xac dinh cac thong tin co trong yeu cau
		direction = rq_data_dir(rq);
		begin_sector = blk_rq_pos(rq);
		if(begin_sector >= vblock_dev.nsectors)
		{
			printk("This is bad request\n");
			goto done;
		}

		num_sectors = blk_rq_cur_sectors(rq);
		if(begin_sector + num_sectors > vblock_dev.nsectors)
		{
			num_sectors = vblock_dev.nsectors - begin_sector;
			printk("Warning: transferring data with %u sectors\n", num_sectors);
		}

		buffer = bio_data(rq->bio);
		if(direction == READ) {
			printk("read %u sectors, start from sector %u\n", num_sectors, begin_sector);
			vblock_dev_read(begin_sector, buffer, num_sectors);
		} else {
			printk("write %u sectors, start from sector %u\n", num_sectors, begin_sector);
			vblock_dev_write(begin_sector, buffer, num_sectors);
		}
	done:
		printk("Xu ly loi");	
	//if(!blk_get_request(rq, res))//(!__blk_end_request_cur(rq, res))
			//blk_get_request(rq, res);//rq = blk_fetch_request(q);
		//__blk_end_request_cur(rq, res)
	}
	printk ("exit vb_handle_request\n");
}

static int vb_open(struct block_device *bdev, fmode_t mode)
{
	//xac dinh xem partition nao duoc truy cap
	unsigned minor = iminor(bdev->bd_inode);
	if(minor > VBLOCK_MINOR_COUNT)
	{
		printk("There is no partition %u\n", minor);
		return -ENODEV;
	}

	printk("open partition %u\n", minor);
	return 0;

}

static void vb_close(struct gendisk *disk, fmode_t mode)
{
	printk("close partition\n");
}

static struct block_device_operations vb_fops = {
	.owner = THIS_MODULE,
	.open = vb_open,
	.release = vb_close,
};

static int __init vblock_drv_init(void)
{
	int ret;

	//khoi tao vblock device
	ret = vblock_dev_init();
	if (ret < 0)
		return ret;
	vblock_dev.nsectors = ret;

	//dang ky voi kernel mot so major cho block device
	ret = register_blkdev(0, VBLOCK_DEVICE_NAME);
	if (ret < 0)
	{
		printk("Unable to get a major number for vblock device\n");
		vblock_dev_cleanup();
		return -EBUSY;
	}
	vblock_dev.major = ret;

	// tao ra mot hang doi chua cac request
	spin_lock_init(&vblock_dev.lock);
	//NOTE: phan duoi comment de chay voi kernel 5.0; can fix sau
	//vblock_dev.queue = blk_init_queue(vb_handle_request, &vblock_dev.lock);
	/*	
	if (vblock_dev.queue == NULL)
	{
		printk("Unable to create a request queue for vblock driver\n");
		unregister_blkdev(vblock_dev.major, VBLOCK_DEVICE_NAME);
		vblock_dev_cleanup();
		return -ENOMEM;
	}
	blk_queue_logical_block_size(vblock_dev.queue, VBLOCK_SECTOR_SIZE);
	*/
	//cap phat bo nho cho cau truc gendisk
	vblock_dev.vb_disk = alloc_disk(VBLOCK_MINOR_COUNT);
	if(!vblock_dev.vb_disk)
	{
		printk("Unable to allocate memory for gen_disk structure\n");
		//blk_cleanup_queue(vblock_dev.queue);
		unregister_blkdev(vblock_dev.major, VBLOCK_DEVICE_NAME);
		vblock_dev_cleanup();
		return -ENOMEM;
	}

	//khoi tao cac bien thanh vien cua cau truc gendisk
	vblock_dev.vb_disk->major = vblock_dev.major;
	vblock_dev.vb_disk->first_minor = VBLOCK_FIRST_MINOR;
	vblock_dev.vb_disk->minors = VBLOCK_MINOR_COUNT;
	vblock_dev.vb_disk->fops = &vb_fops;
	vblock_dev.vb_disk->queue = vblock_dev.queue;
	vblock_dev.vb_disk->private_data = &vblock_dev;
	sprintf(vblock_dev.vb_disk->disk_name, VBLOCK_DEVICE_NAME);
	set_capacity(vblock_dev.vb_disk, vblock_dev.nsectors);

	//dang ky cau truc gendisk voi kernel
	add_disk(vblock_dev.vb_disk);

	printk("initialized virtual block device with major number %d\n", vblock_dev.major);
	return 0;
}

static void __exit vblock_drv_exit(void)
{
	del_gendisk(vblock_dev.vb_disk);
	blk_cleanup_queue(vblock_dev.queue);
	unregister_blkdev(vblock_dev.major, VBLOCK_DEVICE_NAME);
	vblock_dev_cleanup();
}

module_init(vblock_drv_init);
module_exit(vblock_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CNTT - KMA");
MODULE_DESCRIPTION("Minh hoa driver cho thiet bi block ao");
