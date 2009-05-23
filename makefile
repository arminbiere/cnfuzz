CC=gcc
CFLAGS=-Wall -g
all: cnfuzz
cnfuzz: cnfuzz.c makefile
	$(CC) $(CFLAGS) -o $@ cnfuzz.c
clean:
	rm -f cnfuzz
