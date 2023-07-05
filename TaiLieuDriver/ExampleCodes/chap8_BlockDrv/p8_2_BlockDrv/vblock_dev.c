#include <linux/types.h>
#include <linux/vmalloc.h>
#include <linux/string.h>

#include "vblock_dev.h"

static char *storage;

//khoi tao thiet bi vblock
int vblock_dev_init(void)
{
	storage = vmalloc(VBLOCK_SECTOR_SIZE * VBLOCK_NUM_SECTORS);
	if(storage == NULL)
		return -ENOMEM;
	return VBLOCK_NUM_SECTORS;
}

void vblock_dev_cleanup(void)
{
	vfree(storage);
}

//ghi du lieu trong buffer vao num_sectors sector, bat dau tu sector begin_sector
void vblock_dev_write(sector_t begin_sector, char *buffer, unsigned int num_sectors)
{
	char *ptr = storage + begin_sector * VBLOCK_SECTOR_SIZE;
	size_t size = num_sectors * VBLOCK_SECTOR_SIZE;
	memcpy(ptr, buffer, size);
}

//doc du lieu cua num_sectors sector, bat dau tu sector begin_sector vao trong buffer
void vblock_dev_read(sector_t begin_sector, char *buffer, unsigned int num_sectors)
{
	char *ptr = storage + begin_sector * VBLOCK_SECTOR_SIZE;
	size_t size = num_sectors * VBLOCK_SECTOR_SIZE;
	memcpy(buffer, ptr, size);
}
