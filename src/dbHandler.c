#include <leveldb/c.h>
#include <stdio.h>
#include <randomKey.h>
#include <string.h>

leveldb_t *db;
leveldb_options_t *options;
leveldb_readoptions_t *roptions;
leveldb_writeoptions_t *woptions;

int openDB(char *directory) {

 	char *errptr = NULL;

	// Define option structs
	options = leveldb_options_create();
	woptions = leveldb_writeoptions_create();
	roptions = leveldb_readoptions_create();

	leveldb_options_set_create_if_missing(options, 1);
	
	db = leveldb_open(options, directory, &errptr);

	if (errptr != NULL) {
		fprintf(stderr, "Failure opening db %s\n", errptr);
    	return -1;
    }

	return 0;

}

int writeDB(char *key, char *value) {
 	
	char *errptr = NULL;

	size_t keylength = 5;

	if(rdmstr(key, keylength) < 0) {
		perror("Error creating random key: ");
		return -1;
	}

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
		fprintf(stderr, "Failure reading db: %s", errptr);
		leveldb_free(errptr);
		return NULL;
	}

	*(read + readlen) = '\0';

	return read;

}

void closeDB() {
	leveldb_close(db);
}
