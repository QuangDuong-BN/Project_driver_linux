#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
 
int8_t write_buf[1024];
int8_t read_buf[1024];
int main()
{
        int fd;
        char option;
        int numbytes_rd = 0;
        int numbytes_wr = 0;
        printf("**************************************************\n");
        printf("*******user application to test char driver*******\n");
 
        fd = open("/dev/char_device", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
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
                                scanf("  %[^\t\n]s", write_buf);
                                printf("Data Writing ...");
                                numbytes_wr = write(fd, write_buf, strlen(write_buf)+1);
				if (numbytes_wr > 0)
					printf("Done!\n");
				else
					printf("kernel buffer is full, can not write\n");
                                break;
                        case '2':
                                printf("Data Reading ...");
                                numbytes_rd = read(fd, read_buf, 1024);
				if (numbytes_rd > 0) {
	                                printf("Done!\n\n");
                                        printf("Data = %s\n\n", read_buf);
				} else
					printf("kernel buffer is empty, can not read\n");
                                break;
                        case '3':
                                close(fd);
                                exit(1);
                                break;
                        default:
                                printf("Enter Valid option = %c\n",option);
                                break;
                }
        }
        close(fd);
}
