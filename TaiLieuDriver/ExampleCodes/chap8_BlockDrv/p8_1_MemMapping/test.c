#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int fd;
	char option;
	char *area; //chua dia chi vung bo nho mapping
	int size = getpagesize();
	printf("**************************************************\n");
	printf("*******user application to test char driver*******\n");

	fd = open("/dev/char_device", O_RDWR);
	if(fd < 0) {
		printf("Cannot open device file...\n");
		return 0;
	}
	area = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if(area == MAP_FAILED) {
		printf("memory mapping failed\n");
		exit(1);
	}
	close(fd);//sau khi thuc hien memory map co the dong file lai

	while(1) {
		printf("****Please Enter the Option******\n");
		printf("        1. Write               \n");
		printf("        2. Read                 \n");
		printf("        3. Exit                 \n");
		printf("*********************************\n");
		scanf(" %c", &option);
		printf("Your Option = %c\n", option);

		switch(option) {
			case '1':
				printf("Enter the string to write into driver :");
				scanf("  %[^\t\n]s", area); //ghi vao vung memory map
				printf("Done!\n");
				break;
			case '2':
				printf("Data Reading ...");
				printf("\n%s\n", area); //doc tu vung memory map
				break;
			case '3':
				exit(1);
				break;
			default:
				printf("Enter Valid option = %c\n",option);
				break;
		}
	}
}
