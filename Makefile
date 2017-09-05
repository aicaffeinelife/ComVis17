CXX=gcc
CFLGAS=std=c99

all: test.o ppm.o conv.o
	$(CXX) -o tr test.o ppm.o conv.o



test.o: test_random.c  
	$(CXX) -g -std=c99 test_random.c -o test.o -c

ppm.o: ppm.c
	$(CXX) -g -std=c99 ppm.c -o ppm.o -c 

conv.o:conv_lib.c
	$(CXX) -g -std=c99 conv_lib.c -o conv.o -c