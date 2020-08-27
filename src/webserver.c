#include <microhttpd.h>
#include <dbHandler.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct MHD_Daemon *daemon;

char *contentPath;

int getIndex(char **content)
{

	FILE *fs;

	//Create path
	char path[100];
	strcpy(path, contentPath);
	strcat(path, "/index.html\0");

	// Open file
	if (!(fs = fopen(path, "r")))
		return -1;

	//Get size of file
	fseek(fs, 0, SEEK_END);
	unsigned int size = ftell(fs);
	rewind(fs);

	// Read to buffer
	if ((*content = malloc(size + 1)) == NULL)
		return -1;

	if (fread(*content, 1, size, fs) < size)
		perror("Error reading file: ");

	fclose(fs);

	return 0;

}

enum MHD_Result answer_to_connection(
	void *cls,
	struct MHD_Connection *connection,
	const char *url,
	const char *method,
	const char *version,
	const char *upload_data,
	size_t *upload_data_size,
	void **con_cls
) {

	// Suppress "unused parameter" warning
	(void) cls;
	(void) method;
	(void) version;
	(void) upload_data;
	(void) upload_data_size;
	(void) con_cls;

	char *content;
	int status = 200;

	// Response mode, set to MUST_FREE when pointer points to heap
	enum MHD_ResponseMemoryMode mode = MHD_RESPMEM_MUST_FREE;

	struct sockaddr** addr = (struct sockaddr **) MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
	char clientip[INET_ADDRSTRLEN];
	strcpy(clientip, inet_ntoa( ((struct sockaddr_in *) *addr)->sin_addr));

	printf("[%s]\tRequest:\t%s\n", clientip, url);

	// send index.html
	if (strcmp(url, "/") == 0)
	{
		if (getIndex(&content) < 0)
		{
			perror("Error opening file: ");
			content = "An error occured";
			mode = MHD_RESPMEM_PERSISTENT;
			status = 500;
		}
	}

	// Get value from DB
	else
	{
		const char *key = url + 1;
		char *value = readDB(key);

		if (value == NULL)
		{
			content = "Not found\0";
			mode = MHD_RESPMEM_PERSISTENT;
			status = 404;
			free(value);
		}

		else
		{
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

int webserverStart(size_t port, char *path)
{

	//Store vars globally
	contentPath = path;

	daemon = MHD_start_daemon(
		MHD_USE_INTERNAL_POLLING_THREAD,
		port,
		NULL,
		NULL,
		&answer_to_connection,
		NULL,
		MHD_OPTION_END);

	if (daemon == NULL)
		return -1;
	else
		return 0;

}

void webserverStop()
{
	MHD_stop_daemon(daemon);
}
