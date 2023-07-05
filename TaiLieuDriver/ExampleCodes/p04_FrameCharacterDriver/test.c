#include<stdio.h>

#include<stdlib.h>

#include<unistd.h>

#include<sys/ioctl.h>

#include<sys/types.h>

#include<sys/stat.h>

#include<fcntl.h>

#include<sys/select.h>

#include<sys/time.h>

#include<errno.h>

int main(int argc, char** argv)
{

     int fd, fd2;

     int result,rt, wt;
	const int writeResult = 100;

     fd = open(argv[1], 0);    
	fd2 = open(argv[2], 1);

     rt=read(fd,&result,sizeof(result));

     printf("\nKet qua tra ve tu read la %d\n",rt);

	wt=write(fd2,&writeResult,sizeof(writeResult));

     printf("\nKet qua tra ve tu write la %d\n",wt);

     close(fd);

     return 0;
}