CC=gcc
CFLAGS=-pthread -Wall -Wextra -O2 -std=c99 -pedantic -Isrc/include -lleveldb -lmicrohttpd

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

build:
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/catbind src/*.c

# With errors instead of warnings
release:
	mkdir -p bin
	$(CC) $(CFLAGS) -Werror -o bin/catbind src/*.c

# Adds symbols
debug:
	mkdir -p bin
	$(CC) $(CFLAGS) -g -o bin/catbind src/*.c

install:
	install -Dvm 755 bin/catbind $(DESTDIR)$(PREFIX)/bin/catbind
	install -Dvm 644 src/webcontent/* -t $(DESTDIR)$(PREFIX)/share/catbind/webcontent
	install -Dvm 644 README.md LICENSE $(DESTDIR)$(PREFIX)/share/catbind/

	# I have yet to figure out how to do /etc correctly
	install -Dbvm 644 catbind.conf $(DESTDIR)$(PREFIX)/etc/catbind.conf
	install -DCvm 644 catbind.service $(DESTDIR)$(PREFIX)/lib/systemd/system/catbind.service

uninstall:
	rm -v $(DESTDIR)$(PREFIX)/bin/catbind
	rm -rv $(DESTDIR)$(PREFIX)/share/catbind
	rm -v $(DESTDIR)$(PREFIX)/etc/catbind.conf
	rm -v $(DESTDIR)$(PREFIX)/lib/systemd/system/catbind.service

make check:
	./test.sh

clean:
	rm -r bin
