#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
 
int8_t read_buf[1024];
int8_t write_buf[1024]="CNTT.KMA.VN\n";
int main()
{
        int fd;
 
	//mo file thiet bi
        fd = open("/dev/tenDeviceFile", O_RDWR);

        if(fd < 0) {
                printf("Khong mo duoc file...\n");
                return 0;
        }
 
	write(fd, write_buf, strlen(write_buf)+1);

	read(fd, read_buf, 1024);

	printf("Data = %s", read_buf);

	close(fd);
}
