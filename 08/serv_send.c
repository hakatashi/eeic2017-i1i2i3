#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#define  WINDOW_SIZE 100

int main(int argc, char const *argv[]) {
	int ret;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	const int port = atoi(argv[1]);

	const int socket_id = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;

	bind(socket_id, (struct sockaddr *)&address, sizeof(address));

	listen(socket_id, 10);

	struct sockaddr_in client_address;
	socklen_t length = sizeof(struct sockaddr_in);
	int client_socket_id = accept(socket_id, (struct sockaddr *)&client_address, &length);

	close(socket_id);

	int read_bytes;
	do {
		uint8_t data[WINDOW_SIZE];
		read_bytes = fread(data, sizeof(uint8_t), WINDOW_SIZE, stdin);
		if (read_bytes == -1) {
			perror("recv");
			exit(1);
		}

		if (read_bytes != 0) {
			write(client_socket_id, data, read_bytes);
		}
	} while(read_bytes > 0);

	return 0;
}
