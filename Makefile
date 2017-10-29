all: clean a2

a2: a2.c
	mpicc a2.c -o a2 -Wall -g -std=c99

clean:
	rm -f a2