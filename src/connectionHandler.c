#include <stdio.h>
#include <string.h>
#include <dbHandler.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ArgumentStruct.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define MAX_MESSAGE_LENGTH 130000

void *connectionHandler(void *argsptr) {

	struct ArgumentStruct *args = argsptr;

	int sock = args->sock;
	char *domain = args->domain;

	// We concatentate this so set it to 0's
	char *totalBuffer = calloc(1, MAX_MESSAGE_LENGTH);
	char readBuffer[1024];

	// bytes that we have put in the buffer
	int accumulated = 0;

	int read_size;
	while((read_size = read(sock , readBuffer , 1024)) > 0) {
		
		printf("Received %d bytes\n", read_size);

		accumulated += read_size;

		// Limit reached
		if (accumulated > MAX_MESSAGE_LENGTH) {
			char message[] = "Max size is 130.000 characters!\n";
			if (write(sock, message, strlen(message)) < 0)
				perror("Error writing to socket: ");
			if (shutdown(sock, SHUT_RDWR) < 0)
				perror("Error closing socket: ");
		
			return NULL;
		}
		
		// Add new data to total
		strcat(totalBuffer, readBuffer);

		// Assume this was the last packet, stop reading
		if (read_size != 1024) {
			break;
		}

		// Nullify buffer
		memset(readBuffer, '\0', 1024);

		//Otherwise set a timeout

	}

	char key[5];
	writeDB(&key, totalBuffer);
	
	size_t length = strlen(domain) + 15;
	char url[length];
	snprintf(url, length, "http://%s/%s\n", domain, key);

	if (write(sock, url, length) < 0)
		perror("Error writing to socket: ");

	if (shutdown(sock, SHUT_RDWR) < 0)
		perror("Error shutting down socket: ");

	free(totalBuffer);

	return NULL;

}
