CC=gcc
CFLAGS=-Wall -lm
all: cnfuzz
cnfuzz: cnfuzz.c
	$(CC) $(CFLAGS) -o $@ cnfuzz.c
clean:
	rm -f cnfuzz
