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
	-h, --domain		The domain where the service is accessed from
	-d, --data			Directory to store data in

```

## To use catbin:
```SH
echo HELLO WORLD | nc catbin.xyz 5454
```
Alternatively you can setup an alias like this:
```SH
alias catbin="nc catbin.xyz 5454 <"
```
