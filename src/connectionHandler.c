#include <stdio.h>
#include <string.h>
#include <dbHandler.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ArgumentStruct.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_MESSAGE_LENGTH 130000

void connectionHandler(void *argsptr) {

	struct ArgumentStruct *args = argsptr;

	int sock = args->sock;
	char *domain = args->domain;
	int timeout = args->timeout;

	// We concatentate this so set it to 0's
	char *totalBuffer = calloc(1, MAX_MESSAGE_LENGTH);
	char readBuffer[1024];

	// bytes that we have put in the buffer
	int accumulated = 0;

	/*
	Netcat sends data in packets of 1024, sometimes higher when multiple are received as one.
	This is why we assume packets smaller than 1024 to be the last send packet.
	When the last packet received was in fact 1024 or more bytes the connection will be closed via a timeout.
	
	Update:
	As of 9/7/20 catbin now always relies on a timeout.
	In production sometimes netcat sends a smaller packet than 1024 bytes,
	thus assuming any packets smaller than 1024 bytes doesn't work.
	
	*/

 	int read_size;

	/* Set timeout */
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	while(((read_size = read(sock, readBuffer, 1024)) > 0)) {

		printf("Received %d bytes\n", read_size);

		accumulated += read_size;

		// Limit reached (-1 because of the null byte)
		if (accumulated > MAX_MESSAGE_LENGTH - 1) {
			char message[] = "Max size is 130.000 characters!\n";
			if (write(sock, message, strlen(message)) < 0)
				perror("Error writing to socket: ");
			if (shutdown(sock, SHUT_RDWR) < 0)
				perror("Error closing socket: ");
		
			return;
		}
		
		// Add new data to total
		strcat(totalBuffer, readBuffer);

		// Assume this was the last packet, stop reading
		/* This was unreliable in production
		if (read_size < 1024) {
			break;
		} */

		// Nullify buffer
		memset(readBuffer, '\0', 1024);

	}

	if (accumulated > 0) {
		char key[5];
		writeDB(&key, totalBuffer, accumulated);
		
		size_t length = strlen(domain) + 15;
		char url[length];
		snprintf(url, length, "http://%s/%s\n", domain, key);

		if (write(sock, url, length) < 0)
			perror("Error writing to socket: ");
	}

	if (shutdown(sock, SHUT_RDWR) < 0)
		perror("Error shutting down socket: ");

	free(totalBuffer);

	return;

}
