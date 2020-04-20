CC=gcc
CFLAGS=-pthread -Wall -Wextra -O2 -std=c99 -pedantic -lleveldb -lmicrohttpd

build:
	$(CC) $(CFLAGS) -o bin/catbind src/*.c -I src/headers

# Adds symbols
debug:
	$(CC) $(CFLAGS) -g -o bin/catbind src/*.c -I src/headers

install:
	install -Dvm 755 bin/catbind /usr/lib/catbind
	cd /usr/bin; ln -vfs ../lib/catbind catbind; cd -
	install -Dvm 644 src/webcontent/* -t /usr/share/catbind/webcontent

uninstall:
	rm -rv /usr/lib/catbind /usr/bin/catbind /usr/share/catbind/webcontent

clean:
	rm -r bin
