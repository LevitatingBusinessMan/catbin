#!/bin/bash

STR="foo"
PORT="5454"
HOST="localhost"
SRVPORT="8080"

bin/catbind -s $SRVPORT -h "${HOST}:${SRVPORT}" -d "/tmp/catbind" &

# Sleep a second for the server to start
sleep 1

PID=$!

URL=$(echo ${STR} | nc ${HOST} ${PORT} | sed 's/\x0//g')

#compare output
if [ "$(curl -s $URL)" == "$STR" ]; then
	kill $PID
	echo "Test ran succesfully"
	exit 0
else
	echo "Test failed"
	kill $PID
	exit 1
fi

kill $PID
