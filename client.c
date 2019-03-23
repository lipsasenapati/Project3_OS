#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

void error(char *msg) {
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]) {
	int flag;
	int sockfd, portno, ret, clientid;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[1024], msg[100], filename[20];

	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[1]);
		exit(0);
	}
	portno = atoi(argv[2]);
//	clientid = atoi(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		sprintf(msg, "ERROR opening socket");
		error(msg);
	}

	server = gethostbyname(argv[1]);
	if (server == NULL) {
		sprintf(msg, "ERROR, no such host");
		error(msg);
	}

	bzero((char * ) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char * ) &serv_addr.sin_addr.s_addr,
			server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
		sprintf(msg, "ERROR connecting");
		error(msg);
	}

	while (1) {
		printf("Enter input filename: ");
		scanf("%s", filename);
		if (strcmp(filename, "nullfile") == 0) {
			close(sockfd);
			exit(1);
		}

		sprintf(msg, "%s", filename);
		if (write(sockfd, msg, strlen(msg)) < 0) {
			sprintf(msg, "ERROR writing to socket\n");
			error(msg);
		}
		bzero(buffer, 1025);
		flag = 0;
		while (flag == 0) {
			ret = read(sockfd, buffer, 1024);
			if (ret < 0) {
				sprintf(msg, "ERROR reading from socket\n");
				error(msg);
			}
			printf("%s\n", buffer);
			if (strcmp(buffer, "Process had completed sucessfully!!!") == 0) {
				flag = 1;

			}

		}

	}
}
