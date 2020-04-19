#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "headers/connectionHandler.h"
#include "headers/dbHandler.h"
#include "headers/webserver.h"

int dbStrarted = 0;
int webserverStarted = 0;

void beforeExit() {
	if (dbStrarted == 1)
		closeDB();
	
	if (webserverStart == 1)
		webserverStop();
}

int main(int argc, char *argv[]) {

	atexit(beforeExit);

	int port = 5454;
	int webPort = 8080;

	int option;
	while ((option = getopt(argc, argv, ":ps:")) != -1 )  {  
        switch(option) {  
			case 'p':
				port = atoi(optarg);
				break;
			case 's':
				webPort = atoi(optarg);
				break;
			case ':':
				fprintf(stderr, "Missing argument value\n");
				return 1;
        }
	}

	if (webserverStart(webPort) < 0) {
		perror("Error starting webserver: ");
		return 1;
	}
	
	printf("Webserver listening at %d\n", webPort);

	webserverStarted = 1;


 	if (openDB() < 0)
		return 1;

	dbStrarted = 1;


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
		printf("Socket listening at %d\n", port);
	}

	if (listen(socket_desc, 3) < 0) {
		perror("Error: ");
		return 1;
	}

	size_t addr_len = sizeof(server);

	int conn_socket;
	while ((conn_socket = accept(socket_desc, (struct sockaddr *)&server, (socklen_t *)&addr_len))) {

		if (conn_socket <1) {
			perror("Error");
			break;
		}
		
		void connectionHandler(int socket);
		pthread_t thread_id;

		if(pthread_create( &thread_id , NULL ,  connectionHandler , conn_socket) < 0) {
			perror("Error");
			break;
		}

		pthread_detach( thread_id );

	}


}
