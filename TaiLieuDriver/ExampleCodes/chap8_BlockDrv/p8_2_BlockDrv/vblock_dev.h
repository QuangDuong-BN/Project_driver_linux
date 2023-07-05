#define VBLOCK_SECTOR_SIZE 512
#define VBLOCK_NUM_SECTORS 1024

//khoi tao thiet bi vblock
extern int vblock_dev_init(void);

extern void vblock_dev_cleanup(void);

//ghi du lieu trong buffer vao num_sectors sector, bat dau tu sector begin_sector
extern void vblock_dev_write(sector_t begin_sector, char *buffer, unsigned int num_sectors);

//doc du lieu cua num_sectors sector, bat dau tu sector begin_sector vao trong buffer
extern void vblock_dev_read(sector_t begin_sector, char *buffer, unsigned int num_sectors);
