CC=gcc
CFLAGS=-pthread -Wall -Wextra -O2 -std=c99 -pedantic -Isrc/headers 


build:
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/catbind src/*.c -lleveldb -lmicrohttpd

# With errors instead of warnings
release:
	mkdir -p bin
	$(CC) $(CFLAGS) -Werror -o bin/catbind src/*.c -lleveldb -lmicrohttpd
# Adds symbols
debug:
	mkdir -p bin
	$(CC) $(CFLAGS) -g -o bin/catbind src/*.c -lleveldb -lmicrohttpd

install:
	install -Dvm 755 bin/catbind /usr/lib/catbind
	cd /usr/bin; ln -vfs ../lib/catbind catbind; cd -
	install -Dvm 644 src/webcontent/* -t /usr/share/catbind/webcontent

uninstall:
	rm -rv /usr/lib/catbind /usr/bin/catbind /usr/share/catbind

clean:
	rm -r bin
