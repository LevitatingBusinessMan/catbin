#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "headers/connectionHandler.h"
#include "headers/dbHandler.h"

void beforeExit() {
	closeDB();
}

int main(int argc, char *argv[]) {
	
	atexit(beforeExit);

	puts("Setting up database");
 	if (setupDB() > 0)
		return 1;
	
	int port = 5454;
	int option;
	while ((option = getopt(argc, argv, ":p:")) != -1 )  {  
        switch(option) {  
			case 'p':
				port = atoi(optarg);
				break;
			case ':':
				fprintf(stderr, "Missing argument value\n");
				return 1;
        }
	}

	int socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	
	if (socket_desc == -1) {
		perror("Error");
		return 1;
	}

	struct sockaddr_in server;

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	//Bind and check for errors
	if ( bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("Error binding to port");
		return 1;
	} else {
		printf("Binded to port %d\n", port);
	}

	if (listen(socket_desc, 3) < 0) {
		perror("Error");
		return 1;
	} else {
		puts("Listening...");
	}

	size_t addr_len = sizeof(server);

	int conn_socket;
	while ((conn_socket = accept(socket_desc, (struct sockaddr *)&server, (socklen_t *)&addr_len))) {

		if (conn_socket <1) {
			perror("Error");
			break;
		}

		puts("Connection accepted");
		
		void connectionHandler(int socket);
		pthread_t thread_id;

		if(pthread_create( &thread_id , NULL ,  connectionHandler , conn_socket) < 0) {
			perror("Error");
			break;
		}

		pthread_detach( thread_id );

	}


}
