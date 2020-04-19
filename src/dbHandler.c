#include <leveldb/c.h>
#include <stdio.h>
#include "headers/randomKey.h"

leveldb_t *db;
leveldb_options_t *options;
leveldb_readoptions_t *roptions;
leveldb_writeoptions_t *woptions;

int openDB() {

 	char *errptr = NULL;

	// Define option structs
	options = leveldb_options_create();
	woptions = leveldb_writeoptions_create();
	roptions = leveldb_readoptions_create();

	leveldb_options_set_create_if_missing(options, 1);
	
	db = leveldb_open(options, "termbin", &errptr);

	if (errptr != NULL) {
		fprintf(stderr, "Failure opening db %s\n", errptr);
    	return -1;
    }

	return 0;

}

int writeDB(char *key, char *value) {
 	
	char *errptr = NULL;

	size_t keylength = 5;
	rdmstr(key, keylength);

	leveldb_put(db, woptions, key, strlen(key), value, strlen(value), &errptr);

	if (errptr != NULL) {
		fprintf(stderr, "Failure writing to db: %s\n", errptr);
		leveldb_free(errptr);
		return -1;
    }

	return 0;

}

char *readDB(char *key) {

	char *errptr = NULL;
	size_t readlen;

	char *read = leveldb_get(db, roptions, key, strlen(key), &readlen, &errptr);

	if (errptr != NULL) {
		fprintf(stderr, "Failure reading db:", errptr);
		leveldb_free(errptr);
		return NULL;
	}

	return read;

}

int closeDB() {
	leveldb_close(db);
}
