# Catbind
A termbin clone.
To build/install/run the server:
```SH
git clone https://github.com/LevitatingBusinessMan/catbin
make
sudo make install
catbind
```

To use catbin:
```SH
echo HELLO WORLD | nc catbin.xyz 5454
```
Alternatively you can setup an alias like:
```SH
alias catbin="nc localhost 5454 <"
```
