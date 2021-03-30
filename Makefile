CC=gcc
CFLAGS=-pthread -Wall -Wextra -O2 -std=c99 -pedantic -Isrc/include -lleveldb -lmicrohttpd


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
	install -Dvm 755 bin/catbind /usr/lib/catbind
	cd /usr/bin; ln -vfs ../lib/catbind catbind; cd -
	install -Dvm 644 src/webcontent/* -t /usr/share/catbind/webcontent
	install -Dbvm 644 catbind.conf /etc/catbind.conf
	install -DCvm 644 catbind.service /usr/lib/systemd/system/catbind.service

uninstall:
	rm -rv /usr/lib/catbind /usr/bin/catbind /usr/share/catbind

make check:
	./test.sh

clean:
	rm -r bin
