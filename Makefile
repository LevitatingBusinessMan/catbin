CC=gcc
CFLAGS=-pthread -Wall -Wextra -O2 -std=c99 -pedantic -lleveldb -lmicrohttpd

build:
	$(CC) $(CFLAGS) -o bin/server src/*.c -I src/headers

# Adds symbols
debug:
	$(CC) $(CFLAGS) -g -o bin/server src/*.c

clean:
	rm server
