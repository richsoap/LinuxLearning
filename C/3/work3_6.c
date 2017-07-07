#include "apue.h"
#include <fcntl.h>

int main() {
	int fd;
	if ((fd = openat(AT_FDCWD,"test.out",O_RDWR,O_APPEND | O_CREAT)) < 0) 
		err_sys("Open error");
	if (write(fd,"abcdefg",7) != 7)
		err_sys("Write error");
	if (lseek(fd,2,SEEK_SET) == -1)
		err_sys("Lseek error");
	if (write(fd,"test",4) != 4)
		err_sys("Write error");
	close(fd);
	exit(0);
}
