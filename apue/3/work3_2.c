#include <unistd.h>
#include "apue.h"

int mydup2(int fd,int fd2) {
int heighttest;
if(fd == fd2) {
	return fd;
}
heighttest = dup(fd);
if(heighttest > fd2) {
	close(heighttest);
	close(fd2);
	dup(fd);
}
else if(heighttest < fd2){
	mydup2(heighttest,fd2);
	close(heighttest);
}
return fd2;
}
