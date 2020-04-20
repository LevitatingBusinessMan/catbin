# Catbin
A termbin clone.

## To build/install/run the server:
```SH
git clone https://github.com/LevitatingBusinessMan/catbin
sudo apt-get install libmicrohttpd-dev libleveldb-dev -y
make
sudo make install
catbind
```

#### Arguments
```
Usage: catbin [OPTION]...
Launch a catbin server

	-p					Port to listen for nc sockets
	-s					Port to run webserver on
	-w, --webcontent	Directory were files are served from
	-d, --domain		The domain where the service is accessed from

```

## To use catbin:
```SH
echo HELLO WORLD | nc catbin.xyz 5454
```
Alternatively you can setup an alias like this:
```SH
alias catbin="nc localhost 5454 <"
```
