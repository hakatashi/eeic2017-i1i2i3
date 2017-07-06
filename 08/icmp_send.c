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

#define PACKETSIZE (1024 * 20)

struct packet {
	struct icmphdr hdr;
	char msg[PACKETSIZE];
};

struct receive_packet {
	struct ip ip_header;
	struct icmphdr hdr;
	char msg[PACKETSIZE];
};

/* fd から 必ず n バイト読み, bufへ書く.
	n バイト未満でEOFに達したら, 残りは0で埋める.
	fd から読み出されたバイト数を返す */
ssize_t read_n(int fd, ssize_t n, void * buf) {
	ssize_t re = 0;
	while (re < n) {
		ssize_t r = read(fd, buf + re, n - re);
		if (r == -1) {
			perror("read");
			exit(1);
		}
		if (r == 0) break;
		re += r;
	}
	memset(buf + re, 0, n - re);
	return re;
}

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

void ping(const char *host, const char *buf, int size) {
	int ret;
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

	memset(packet_id.msg, 0, sizeof(packet_id.msg));
	memcpy(packet_id.msg, buf, size);

	packet_id.hdr.un.echo.sequence = 0;
	packet_id.hdr.checksum = checksum(&packet_id, sizeof(packet_id));

	if (sendto(socket_id, &packet_id, sizeof(packet_id), 0, (struct sockaddr*)&address, sizeof(address)) <= 0) {
		perror("sendto");
		return;
	}
}

int main(int argc, char const *argv[]) {
	int ret, i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <host>\n", argv[0]);
		exit(1);
	}

	const char *host = argv[1];

	char buf[PACKETSIZE];
	while (1) {
		ret = read_n(0, PACKETSIZE, buf);
		printf("read: %d bytes\n", ret);

		if (ret == 0) {
			break;
		}

		ping(host, buf, PACKETSIZE);
		usleep(200 * 1000);
	}

	return 0;
}
