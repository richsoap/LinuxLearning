cxx = g++
tail = -std=c++11

so:
	$(cxx) stopwatch.cpp $(tail) -fPIC -shared -o libstopwatch.so
a:
	$(cxx) stopwatch.cpp $(tail) -c
	ar crv libstopwatch.a stopwatch.o

all: 
	$(cxx) main.cpp libstopwatch.a $(tail) -o test

