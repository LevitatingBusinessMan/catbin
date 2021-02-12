PORT="5454"
HOST="localhost"
SRVPORT="8080"

make debug

gdb --args bin/catbind -s $SRVPORT -h "${HOST}:${SRVPORT}" -d "/tmp/catbind" 
