#include <stdio.h>
#include <string.h>
#include <dbHandler.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ArgumentStruct.h>

void *connectionHandler(void *argsptr) {

	struct ArgumentStruct *args = argsptr;

	int sock = args->sock;
	char *domain = args->domain;

	int maxMessageLength = 10000;
	int bufferSize = maxMessageLength + 1;

	char *readBuffer[bufferSize];
	
	int read_size;
	while( (read_size = read(sock , readBuffer , bufferSize)) > 0) {
		
 		if (read_size > maxMessageLength) {

			char message[] = "Max size is 10.000 characters!\n";
			if (write(sock, message, strlen(message)) < 0)
				perror("Error writing to socket: ");
			if (shutdown(sock, SHUT_RDWR) < 0)
				perror("Error closing socket: ");		
		}

		char key[5];
		printf("Received %d bytes\n", read_size);
		writeDB(&key, readBuffer);
		
		size_t length = strlen(domain) + 15;
		char url[length];
		snprintf(url, length, "http://%s/%s\n", domain, key);

		if (write(sock, url, length) < 0)
			perror("Error writing to socket: ");
	
		if (shutdown(sock, SHUT_RDWR) < 0)
			perror("Error shutting down socket: ");
		
		close(sock);

	}

	return NULL;

}
