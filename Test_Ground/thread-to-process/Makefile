.PHONY: all

all: 
	gcc procthread.c -o procthread.o -c
	gcc threadprocess.c -o threadprocess.o -c
	gcc -pthread -o procthread procthread.o threadprocess.o

	rm *.o
	./procthread
