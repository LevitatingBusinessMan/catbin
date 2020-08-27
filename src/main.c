#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <connectionHandler.h>
#include <dbHandler.h>
#include <webserver.h>
#include <ArgumentStruct.h>

int dbStrarted = 0;
int webserverStarted = 0;

void beforeExit() {
	if (dbStrarted == 1)
		closeDB();
	
	if (webserverStarted == 1)
		webserverStop();
}

int main(int argc, char *argv[]) {

	atexit(beforeExit);

	int helpPage = 0;

	int port = 5454;
	int webPort = 80;
	int timeout = 3;
	int max_length = 1048576;

	char *webcontentPath = "/usr/share/catbind/webcontent";
	char *domain = "catbin.xyz";
	char *databaseDirectory = "/var/lib/catbind";

	struct option long_options[] = {
		{"help", no_argument, &helpPage, 1},
		{"port", required_argument, 0, 'p'},
		{"wport", required_argument, 0, 's'},
		{"webcontent", required_argument, 0, 'w'},
		{"domain", required_argument, 0, 'h'},
		{"data", required_argument, 0, 'd'},
		{"timeout", required_argument, 0, 't'},
		{"maxlength", required_argument, 0, 'm'}
	};

	int option;
	while ((option = getopt_long(argc, argv, ":p:s:w:h:d:t:m:", long_options, NULL)) != -1 )  {  
        switch(option) {  
			case 'p':
				port = atoi(optarg);
				break;
			case 's':
				webPort = atoi(optarg);
				break;
			case 'w':
				webcontentPath = optarg;
				break;
			case 'h':
				domain = optarg;
				break;
			case 'd':
				databaseDirectory = optarg;
				break;
			case 't':
				timeout = atoi(optarg);
				break;
			case 'm':
				max_length = atoi(optarg);
				break;
			case ':':
				fprintf(stderr, "Missing argument value\n");
				return 1;
        }
	}

	if (helpPage == 1) {
		printf(
		"Usage: catbind [OPTION]...\n"
		"Launch a catbin server\n"
		"\n"
		"\t    --help          Show this help page\n"
		"\t-p, --port          Port to listen for nc sockets\n"
		"\t-s, --wport         Port to run webserver on\n"
		"\t-w, --webcontent    Directory were files are served from\n"
		"\t-h, --domain        The domain where the service is accessed from\n"
		"\t-d, --data          Directory to store data in\n"
		"\t-t, --timeout       Time to wait before closing socket in seconds\n"
		"\t-m, ---maxlength    Max length for a paste in bytes\n");
	
		exit(0);
	}

	// If string starts with a space remove it
	if (*webcontentPath == ' ')
		webcontentPath++;
	
	if (*domain == ' ')
		domain++;
	
	if (*databaseDirectory == ' ')
		databaseDirectory++;

 	if (openDB(databaseDirectory) < 0)
		return 1;

	printf("Opened db at %s\n", databaseDirectory);

	dbStrarted = 1;

	if (webserverStart(webPort, webcontentPath) < 0) {
		perror("Error starting webserver: ");
		return 1;
	}
	
	printf (
		"Webserver listening at %d\n"
		"Serving from %s\n"
		"Domain: %s\n", 
		webPort, webcontentPath, domain
	);

	webserverStarted = 1;

	int socket_fdesc = socket(AF_INET , SOCK_STREAM , 0);
	
	if (socket_fdesc == -1) {
		perror("Error");
		return 1;
	}

	struct sockaddr_in server;

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	int opton_value = 1;
	if (setsockopt(socket_fdesc, SOL_SOCKET, SO_REUSEADDR, &opton_value , sizeof(int)) < 0) {
		perror("Error setting socket_fdesc option");
		return 1;
	}

	//Bind and check for errors
	if ( bind(socket_fdesc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("Error binding to port");
		return 1;
	}
	
	printf("Socket listening at %d\n", port);

	if (listen(socket_fdesc, 3) < 0) {
		perror("Error: ");
		return 1;
	}

	size_t addr_len = sizeof(server);

	int conn_socket;
	while ((conn_socket = accept(socket_fdesc, (struct sockaddr *)&server, (socklen_t *)&addr_len))) {

		if (conn_socket <1) {
			perror("Error");
			break;
		}
		
		pthread_t thread_id;

		struct ArgumentStruct args;

		args.sock = conn_socket;
		args.domain = domain;
		args.timeout = timeout;
		args.max_length = max_length;

		if(pthread_create( &thread_id , NULL ,  connectionHandler , &args) < 0) {
			perror("Error");
			break;
		}

		pthread_detach( thread_id );

	}


}
