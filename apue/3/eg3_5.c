#include "apue.h"

#define BUFFSIZE 4086
/*
�����ñ�׼����ͱ�׼�����ʵ�ָ���
*/
int main() {
	int n;
	char buffer[BUFFSIZE];
	while((n=read(STDIN_FILENO,buf,BUFFSIZE))>0)
		if(write(STDOUT_FILENO,buf,n) != n)
			err_sys("write error");
	
	if(n<0)
		err_sys("read error");

	exit(0);
	}
