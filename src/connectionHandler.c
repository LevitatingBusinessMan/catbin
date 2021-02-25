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
	int secure = args->secure;

	struct sockaddr_in addr;
	socklen_t addr_size;
	if (getpeername(sock, (struct sockaddr *)&addr, &addr_size) > 0) {
		perror("Error:");
		return;
	}

	char clientip[INET_ADDRSTRLEN];
	strcpy(clientip, inet_ntoa(addr.sin_addr));
	//inet_ntop(AF_INET, addr.sin_addr, clientip, sizeof clientip);


	char *totalBuffer = malloc(max_length+1);
	char readBuffer[1024];

	// Make first byte a null-byte to assure strcat will work
	totalBuffer[0] = '\0';

	// Bytes that we have put in the buffer
	int accumulated = 0;

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
		strncat(totalBuffer, readBuffer, read_size);
	}

	if (accumulated > 0) {
		char key[5];
		writeDB(&key, totalBuffer, accumulated);
		
		size_t length = strlen(domain) + 15 + secure;
		char url[length];
		if (secure) snprintf(url, length, "https://%s/%s\n", domain, key);
		else snprintf(url, length, "http://%s/%s\n", domain, key);

		if (write(sock, url, length) < 0)
			perror("Error writing to socket: ");
	}

	if (shutdown(sock, SHUT_RDWR) < 0)
		perror("Error shutting down socket: ");

	free(totalBuffer);

	return;

}
