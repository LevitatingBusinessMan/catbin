# Catbin
A [termbin](https://github.com/solusipse/fiche) clone.

## To build/install/run the server:
```SH
git clone https://github.com/LevitatingBusinessMan/catbin
cd catbin
sudo apt-get install libmicrohttpd-dev libleveldb-dev -y
make
sudo make install
catbind
```

You can also use the systemd service:
```
sudo systemctl start catbind
```
Configuration for the systemd service is read from `/etc/catbind.conf`

#### Arguments
```
Usage: catbind [OPTION]...
Launch a catbin server

        --help          Show this help page
    -p, --port          Port to listen for nc sockets
    -s, --wport         Port to run webserver on
    -w, --webcontent    Directory were files are served from
    -h, --domain        The domain where the service is accessed from
    -d, --data          Directory to store data in
    -t, --timeout       Time to wait before closing socket in seconds
    -m, --maxlength     Max length for a paste in bytes
    -e, --secure        Defines if the url returned uses https
    -l, --whitelist     A file containing whitelisted IP addresses

```

## To use catbin:
```SH
echo HELLO WORLD | nc catbin.xyz 5454
```
Alternatively you can setup an alias like this:
```SH
alias catbin="nc catbin.xyz 5454 <"
```

## Using Docker
```
git clone https://github.com/LevitatingBusinessMan/catbin
cd catbin
docker build . -t catbin
docker create volume catbin
docker run -p 5454:5454 -p 80:80 -v catbin:/var/lib/catbind -dt catbin 
```
