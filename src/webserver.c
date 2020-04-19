#include <microhttpd.h>
#include "headers/dbHandler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct MHD_Daemon *daemon;

int answer_to_connection (
	void *cls, struct MHD_Connection *connection,
	const char *url,
	const char *method,
	const char *version,
	const char *upload_data,
	size_t *upload_data_size,
	void **con_cls
)
{

	char *content;
	int status = 200;

	// Response mode, set to MUST_FREE when pointer points to heap
	enum MHD_ResponseMemoryMode mode = MHD_RESPMEM_MUST_FREE;

	// send index.html
	if (url == "/") {
		if (getIndex(&content, &status) < 0) {
			perror("Error opening file: ");
			content  = "An error occured";
			mode = MHD_RESPMEM_PERSISTENT;
			status = 500;
		}
	}
	
	// Get value from DB
	else {
		const char *key = url + 1;
		char *value = readDB(key);

		if (value == NULL) {
			content = "Not found";
			mode = MHD_RESPMEM_PERSISTENT;
			status = 500;
		}

		else {
			content = value;
		}

	}
	
	struct MHD_Response *response;
	response = MHD_create_response_from_buffer(strlen(content), content, mode);

	int ret = MHD_queue_response(connection, status, response);

	if (ret == MHD_NO)
		perror("Error queing response: ");

	MHD_destroy_response(response);

	return ret;

}

int getIndex(char **content, int *status) {

	FILE *fs;

	// Open file
	if (!(fs = fopen("src/webcontent/index.html", "r")))
		return -1;
	
	//Get size of file
 	fseek(fs, 0, SEEK_END);
	int size = ftell(fs);
	//rewind(fs);
	fseek(fs, 0, SEEK_SET);

	// Read to buffer
	if ((*content = malloc(size + 1)) == NULL)
		return -1;
	
	if (fread(*content, 1, size, fs) < 0)
		return -1;
	
	fclose(fs);

	return 0;

}

int webserverStart(size_t port) {
	
	daemon = MHD_start_daemon (
		MHD_USE_INTERNAL_POLLING_THREAD,
		port,
		NULL,
		NULL,
		&answer_to_connection,
		NULL,
		MHD_OPTION_END
	);

	if (daemon == NULL)
		return -1;
	else
		return 0;

}

int webserverStop() {
	MHD_stop_daemon(daemon);
}
