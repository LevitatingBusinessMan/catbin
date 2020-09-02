#include <stdio.h>
#include <string.h>
#include <dbHandler.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ArgumentStruct.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <arpa/inet.h>

void connectionHandler(void *argsptr) {

	struct ArgumentStruct *args = argsptr;

	int sock = args->sock;
	char *domain = args->domain;
	int timeout = args->timeout;
	int max_length = args->max_length;

	struct sockaddr_in addr;
	socklen_t addr_size;
	if (getpeername(sock, (struct sockaddr *)&addr, &addr_size) > 0) {
		perror("Error:");
		return;
	}

	char clientip[INET_ADDRSTRLEN];
	strcpy(clientip, inet_ntoa(addr.sin_addr));
	//inet_ntop(AF_INET, addr.sin_addr, clientip, sizeof clientip);

	// We concatentate this so set it to 0's
	char *totalBuffer = calloc(1, max_length);
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

		printf("[%-15s] TCP: Received %d bytes\n", clientip, read_size);

		accumulated += read_size;

		// Limit reached (-1 because of the null byte)
		if (accumulated > max_length - 1) {
			
			char message[50];
			snprintf(message, 50, "Max size is %d characters!\n", max_length);
			
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
