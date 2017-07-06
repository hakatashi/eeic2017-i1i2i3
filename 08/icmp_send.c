#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <resolv.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>

#define PACKETSIZE 256

struct packet {
	struct icmphdr hdr;
	char msg[PACKETSIZE - sizeof(struct icmphdr)];
};

struct receive_packet {
	struct ip ip_header;
	struct icmphdr hdr;
	char msg[PACKETSIZE - sizeof(struct icmphdr)];
};

unsigned short checksum(void *b, int len) {
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;

	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

int main(int argc, char const *argv[]) {
	int ret, i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <host>\n", argv[0]);
		exit(1);
	}

	const char *host = argv[1];

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = 0;

	ret = inet_aton(host, &(address.sin_addr));
	if (ret == 0) {
		perror("inet_aton");
		exit(1);
	}

	const int socket_id = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_id < 0) {
		perror("socket");
		return;
	}

	const int ttl = 255;
	if (setsockopt(socket_id, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
		perror("Set TTL option");
		return;
	}

	if (fcntl(socket_id, F_SETFL, O_NONBLOCK) != 0) {
		perror("Request nonblocking I/O");
		return;
	}

	struct packet packet_id;
	bzero(&packet_id, sizeof(packet_id));

	packet_id.hdr.type = ICMP_ECHO;
	const int pid = getpid();
	packet_id.hdr.un.echo.id = pid;
	printf("pid: %d\n", pid);

	memset(packet_id.msg, 0, sizeof(packet_id.msg));
	strcpy(packet_id.msg, "hello google");
	printf("***sent message!***\n");
	printf("%s\n", packet_id.msg);

	packet_id.hdr.un.echo.sequence = 0;
	packet_id.hdr.checksum = checksum(&packet_id, sizeof(packet_id));

	if (sendto(socket_id, &packet_id, sizeof(packet_id), 0, (struct sockaddr*)&address, sizeof(address)) <= 0) {
		perror("sendto");
		return;
	}

	sleep(1);

	struct sockaddr_in receive_address;
	struct receive_packet receive_packet_id;
	int receive_address_len = sizeof(receive_address);
	if (recvfrom(socket_id, &receive_packet_id, sizeof(receive_packet_id), 0, (struct sockaddr*)&receive_address, &receive_address_len) > 0) {
		printf("***got message!***\n");
		printf("%s\n", receive_packet_id.msg);
	}

	return 0;
}
