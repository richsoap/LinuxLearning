#include "apue.h"
#include <stdlib.h>

int main() {
	char* temppoint = getenv("PATH");
	  printf("PATH :%s",temppoint);
	return 0;
}
