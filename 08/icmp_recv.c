#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <resolv.h>
#include <string.h>
#include <strings.h>
#include <opus.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>

#define PACKETSIZE 0x2000
#define SAMPLE_RATE 24000
#define MAX_FRAME_SIZE 0x100
#define BITRATE (16 * 1024)
#define CHANNELS 1
#define APPLICATION OPUS_APPLICATION_AUDIO

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
	OpusDecoder *decoder;
  int error;
	opus_int16 output[BITRATE*100];
	unsigned char pcm_bytes[MAX_FRAME_SIZE*CHANNELS*2];

	const int socket_id = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_id < 0) {
		perror("socket");
		exit(1);
	}

	decoder = opus_decoder_create(SAMPLE_RATE, CHANNELS, &error);
   if (error<0)
   {
      fprintf(stderr, "failed to create decoder: %s\n", opus_strerror(error));
      return EXIT_FAILURE;
   }

	while(1){
		unsigned char buf[PACKETSIZE * sizeof(uint16_t)];
		struct sockaddr_in address;
		int address_len = sizeof(address);
		const int n = recvfrom(socket_id, buf, sizeof(buf), 0, (struct sockaddr*)&address, &address_len);

		if (n == -1) {
			perror("recvfrom");
			exit(1);
		}

		struct iphdr *ip_header = (struct iphdr*)buf;
		struct icmphdr *icmp_header = (struct icmphdr*)(buf + ip_header->ihl * 4);

		int message_pointer = 0;

		for(i=0;i<20;i++){

			int frame_size = buf[ip_header->ihl * 4 + sizeof(icmp_header) + message_pointer];
			printf("%d\n", message_pointer);

			printf("length: %d\n", frame_size);
			int j;
			 for (j = 0; j < frame_size; j++) {
				      printf("%02x ", buf[ip_header->ihl * 4 + sizeof(icmp_header) + message_pointer + j + 1]);
			     if (j % 16 == 15) puts("");
				  }
			 puts("");

			opus_int32 data_size = opus_decode(decoder,
				buf + ip_header->ihl * 4 + sizeof(icmp_header) + message_pointer + 1,
		 												 frame_size, output, BITRATE*100, 0);
		  if (data_size<0)
      {
         fprintf(stderr, "decoder failed: %s\n", opus_strerror(error));
         return EXIT_FAILURE;
    }

		message_pointer += frame_size + 1;


		for(j=0;j<CHANNELS*data_size;j++)
      {
         pcm_bytes[2*j]=output[j]&0xFF;
         pcm_bytes[2*j+1]=(output[j]>>8)&0xFF;
    }

	  //write(1,pcm_bytes,CHANNELS*data_size*2);
	}

	}

	return 0;
}
