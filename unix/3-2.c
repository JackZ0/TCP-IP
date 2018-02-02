#include "apue.h"
#include <fcntl.h>
char buf1[]="abcdefghijklmnopq";
char buf2[]="ABCDEFGHIJKLMNOPQ";

int main(void)
{
	int fd;
	if((fd = creat("file.hole",FILE_MODE))<0)
		err_sys("create error");
	
	if(write(fd,buf1,10)!=10)
		err_sys("buf1 write errror!\n");
	
	if(lseek(fd,16384,SEEK_SET) == -1)
		err_sys("lseek error");
	
	if(write(fd,buf2,10)!=10)
		err_sys("buf2 wirte error");

	exit(0);
}
