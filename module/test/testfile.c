#include <stdio.h>
typedef struct test {
	int dd;
	int pp;
	int cc;
};

int main() {
	test test1;
	test1.dd =  10;
	printf(sizeof(test));
	printf("\n%d",test1.dd);
	return 0;

}
