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

#define SAMPLES_PER_FRAME 480
#define FRAMES_PER_PACKET 50
#define PACKETSIZE 0x2000
#define SAMPLE_RATE 24000
#define MAX_FRAME_SIZE 0x100
#define BITRATE (16 * 1024)

#define ICMP_IDENTIFIER 0xDEAD

int main(int argc, const char *argv[]) {
	OpusDecoder *decoder;
	int error;

	if (argc != 1) {
		fprintf(stderr, "Usage: %s\n", argv[0]);
		exit(1);
	}

	const int socket_id = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_id < 0) {
		perror("socket");
		exit(1);
	}

	decoder = opus_decoder_create(SAMPLE_RATE, 1, &error);
	if (error != OPUS_OK) {
		fprintf(stderr, "failed to create decoder: %s\n", opus_strerror(error));
		return EXIT_FAILURE;
	}

	uint16_t current_sequence_number = 0;

	while (1) {
		unsigned char buf[PACKETSIZE * sizeof(uint16_t)];
		struct sockaddr_in address;
		unsigned int address_len = sizeof(address);

		const int n = recvfrom(socket_id, buf, sizeof(buf), 0, (struct sockaddr*)&address, &address_len);
		if (n == -1) {
			perror("recvfrom");
			exit(1);
		}

		struct iphdr *ip_header = (struct iphdr*)buf;
		struct icmphdr *icmp_header = (struct icmphdr*)(buf + ip_header->ihl * 4);
		uint8_t *icmp_body = buf + ip_header->ihl * 4 + sizeof(icmp_header);

		fprintf(stderr, "Receive: %d bytes, ID = 0x%04X, Seq = %d\n", n, icmp_header->un.echo.id, icmp_header->un.echo.sequence);

		current_sequence_number = icmp_header->un.echo.sequence;

		int message_pointer = 0;
		unsigned int total_output = 0;

		for (int i = 0; i < FRAMES_PER_PACKET; i++) {
			int frame_size = icmp_body[message_pointer];

			uint8_t *output_data = (uint8_t *)malloc(frame_size * sizeof(uint8_t));
			memcpy(output_data, icmp_body + message_pointer + 1, frame_size * sizeof(uint8_t));

			opus_int16 decoded_data[SAMPLES_PER_FRAME];
			opus_int32 data_size = opus_decode(decoder, output_data, frame_size, decoded_data, SAMPLES_PER_FRAME, 0);

			message_pointer += frame_size + 1;

			// LE => BE
			uint8_t pcm_bytes[SAMPLES_PER_FRAME * sizeof(opus_int16)];
			for (int j = 0; j < data_size; j++) {
				pcm_bytes[2 * j] = decoded_data[j] & 0xFF;
				pcm_bytes[2 * j + 1] = (decoded_data[j] >> 8) & 0xFF;
			}

			write(1, pcm_bytes, data_size * sizeof(opus_int16));
			total_output += data_size * sizeof(opus_int16);
		}

		fprintf(stderr, "Write: %u bytes\n", total_output);
	}

	return 0;
}
