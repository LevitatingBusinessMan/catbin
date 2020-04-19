#include <stdio.h>
#include "headers/dbHandler.h"

void connectionHandler(int sock) {

	int maxMessageLength = 10000;
	int bufferSize = maxMessageLength + 1;

	char *readBuffer[bufferSize];
	
	int read_size;
	while( (read_size = read(sock , readBuffer , bufferSize)) > 0) {
		
 		if (read_size > maxMessageLength) {

			char message[] = "Max size is 10.000 characters!";
			write(sock, message, strlen(message));
			close(sock);
		}

		char *key[5];
		printf("Received %d bytes\n", read_size);
		writeDB(key, readBuffer);
		
		write(sock, key, strlen(key));
		close(sock);

	}

	puts("Connection closed");

}
