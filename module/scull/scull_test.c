#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
	int fnum = 0;
	printf("init number is %d\n",fnum);
	fnum = open("/dev/scull",O_RDWR);
	printf("file number is %d\n",fnum);
	char buffer[] = "hello world";
	write(fnum,buffer,11);
	buffer[0] ='p';
	read(fnum,buffer,11);
	printf("%s\n",buffer);
	close(fnum);
	return 0;
}
