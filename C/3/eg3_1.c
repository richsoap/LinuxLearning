#include "apue.h"
/*
�����ļ���ƫ����
*/
int main() {
	int dig;
if ((dig = lseek(STDIN_FILENO,0,SEEK_CUR)) == -1) 
	printf("cannot seek\n");
else
	printf("seek OK, the number is \u",dig);
exit(0);
}
