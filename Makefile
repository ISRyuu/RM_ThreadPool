
#Simple Makefile
CC = clang 
CFLAGS = -Wall

all: main clean

main: main.o rm_threadpool.o
	${CC} -o main main.o rm_threadpool.o

main.o: main.c
	${CC} -c main.c

rm_threadpool.o: rm_threadpool.c rm_threadpool.h
	${CC} -c rm_threadpool.c

clean:
	rm -rf *o
