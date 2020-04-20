#include <stdio.h>
#include <string.h>
#include "headers/dbHandler.h"
#include <unistd.h>
#include <sys/socket.h>

void *connectionHandler(void *sockptr) {

	int sock = *(int *)sockptr;

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
		
		size_t length = 25;
		char url[length];
		snprintf(url, length, "http://catbin.xyz/%s\n", key);

		if (write(sock, url, length) < 0)
			perror("Error writing to socket: ");
	
		if (shutdown(sock, SHUT_RDWR) < 0)
			perror("Error closing socket: ");

	}

	return NULL;

}
