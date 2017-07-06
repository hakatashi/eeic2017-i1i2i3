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

	ret = inet_aton(host, &(address.sin_addr));
	if (ret == 0) {
		perror("inet_aton");
		exit(1);
	}

	ret = connect(socket_id, (struct sockaddr *)&address, sizeof(address));
	if (ret == -1) {
		perror("connect");
		exit(1);
	}

	int read_bytes;
	do {
		char data[WINDOW_SIZE];
		read_bytes = recv(socket_id, data, WINDOW_SIZE, 0);
		if (read_bytes == -1) {
			perror("recv");
			exit(1);
		}

		if (read_bytes != 0) {
			printf("%.*s", read_bytes, data);
		}
	} while(read_bytes > 0);

	return 0;
}
