#!/bin/sh
PORT="5454"
HOST="localhost"
SRVPORT="8080"
TIMEOUT="1"

make debug

gdb --args bin/catbind -s $SRVPORT -h "${HOST}:${SRVPORT}" -d "/tmp/catbind" -t $TIMEOUT
