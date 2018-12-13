#include "stopwatch.h"
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
using std::to_string;
Stopwatch::Stopwatch(uint64_t _number, uint64_t _trigger, string _filename) {
    number = _number<1?1:_number;
    records = vector<list<uint64_t>>(number);
    start = vector<uint64_t>(number);
    count = vector<uint64_t>(number);
    trigger = _trigger<1?1:_trigger;
    filename = _filename;
}
Stopwatch::~Stopwatch() {}

void Stopwatch::reset(uint64_t code) {
    if(code >= number)
        return;
    gettimeofday(&timestamp, NULL);
    start[code] = (uint64_t)timestamp.tv_sec*1000000 + timestamp.tv_usec;
}
void Stopwatch::record(uint64_t code) {
    if(code >= number)
        return;
    gettimeofday(&timestamp, NULL);
    records[code].push_back((uint64_t)timestamp.tv_sec*1000000 + timestamp.tv_usec - start[code]);
    count[code] ++;
    if(count[code] >= trigger) {
        count[code] = 0;
        flush(records[code], code);
    }
}

void Stopwatch::flush(list<uint64_t>& buffer, int code) {
    FILE* fp = fopen((filename + to_string(code)).c_str(), "a+");
    for(auto num:buffer) {
        fprintf(fp, "%lu ", num);
    }
    buffer.clear();
    fclose(fp);
}

void Stopwatch::resetAll() {
    gettimeofday(&timestamp, NULL);
    for(int i = 0;i < number;i ++)
        start[i] = (uint64_t)timestamp.tv_sec*1000000 + timestamp.tv_usec;
}
