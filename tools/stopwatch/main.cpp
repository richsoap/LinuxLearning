#include <iostream>
#include <unistd.h>
#include "stopwatch.h"
using namespace std;
int main() {
    Stopwatch watch(10, 100, "testfile");
    int table[] = {1,2,3,4,5,6,7,8,9,10};

    for(int i = 0;i < 1001;i ++) {
        for(int p = 0;p < 10;p ++) {
            watch.reset(p);
            usleep(table[p] * 1000);
            watch.record(p);
        }
    }
    return 0;
}
