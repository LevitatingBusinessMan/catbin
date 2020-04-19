CC=gcc
CFLAGS=-pthread -Wall -Wextra -O2 -std=c99 -pedantic -lleveldb

makeserver:
	$(CC) $(CFLAGS) -o bin/server src/*.c

clean:
	rm server
