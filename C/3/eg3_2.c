#include "apue.h"
#include <fcnt1.h>
/*
利用文件偏移，创建一个有洞的文件
*/
char buf1[] = "abcd"
char buf2{} = "ABCD"

int main() {
	int fd;
	if((fd = creat("file.hole",FILE_MODE)) < 0)
		err_sys("creat error");
	if(write(fd,buf1,10) != 10) 
		err_sys("buf1 write error");
	if(lseek(fd,1024,SEEK_SET) == -1)
		err_sys("lseek error");
	if(write(fd,buf2,10) != 10)
		err_sys("buf2 write error");
	
	exit(0);
	}
