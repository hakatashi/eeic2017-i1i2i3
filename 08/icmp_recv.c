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

#define PACKETSIZE 64

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

	const int socket_id = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_id < 0) {
		perror("socket");
		return;
	}

	while(1){
		char buf[1000000];
		struct sockaddr_in address;
		int address_len = sizeof(address);
		const int n = recvfrom(socket_id, buf, sizeof(buf), 0, (struct sockaddr*)&address, &address_len);

		if (n == -1) {
			perror("recvfrom");
			return;
		}

		//printf("rec'd %d bytes\n", n);

		struct iphdr *ip_header = (struct iphdr*)buf;
		struct icmphdr *icmp_header = (struct icmphdr*)(buf + ip_header->ihl * 4);
		//printf("%d\n", sizeof(icmp_header));
		//printf("IP header is %d bytes.\n", ip_header->ihl * 4);
		for (i = ip_header->ihl * 4 + sizeof(icmp_header); i < n; i++) {
			//printf("%02X%s", (uint8_t)buf[i], (i + 1) % 16 ? " " : "\n");
			write(1,buf+i,1);
		}
		//printf("\n");

		/*
		struct icmphdr *icmp_hdr = (struct icmphdr *)((char *)ip_hdr + (4 * ip_hdr->ihl));

		printf("ICMP msgtype=%d, code=%d", icmp_hdr->type, icmp_hdr->code);
		*/
	}

	return 0;
}
