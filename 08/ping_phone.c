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
#include <assert.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>

#define SAMPLES_PER_FRAME 480
#define FRAMES_PER_PACKET 50
#define MAX_FRAME_SIZE 0x100
#define SAMPLE_RATE 24000
#define BITRATE (16 * 1024)
#define PACKETSIZE 0x2000

enum Mode {
	MODE_HOST,
	MODE_CLIENT,
};

#define ICMP_IDENTIFIER 0xDEAD

struct packet {
	struct icmphdr hdr;
	char msg[MAX_FRAME_SIZE * FRAMES_PER_PACKET];
};

/* fd から 必ず n バイト読み, bufferへ書く.
        n バイト未満でEOFに達したら, 残りは0で埋める.
        fd から読み出されたバイト数を返す */
ssize_t read_n(int fd, ssize_t n, void * buffer) {
	ssize_t re = 0;
	while (re < n) {
		ssize_t r = read(fd, buffer + re, n - re);
		if (r == -1) {
			perror("read");
			exit(1);
		}
		if (r == 0) break;
		re += r;
	}
	memset(buffer + re, 0, n - re);
	return re;
}

unsigned short checksum(void *b, int len) {
	unsigned short *buffer = b;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buffer++;
	if (len == 1)
		sum += *(unsigned char*)buffer;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

void ping(struct sockaddr_in *address, const uint8_t *buffer, int size, uint16_t sequence_number) {
	int ret;

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
	packet_id.hdr.un.echo.id = ICMP_IDENTIFIER;

	memset(packet_id.msg, 0, sizeof(packet_id.msg));
	memcpy(packet_id.msg, buffer, size);

	packet_id.hdr.un.echo.sequence = sequence_number;
	packet_id.hdr.checksum = checksum(&packet_id, sizeof(packet_id.hdr) + size);

	ret = sendto(socket_id, &packet_id, sizeof(packet_id.hdr) + size, 0, (struct sockaddr*)address, sizeof(*address));
	if (ret <= 0) {
		perror("sendto");
		exit(1);
	}
}

void await_connection(struct sockaddr_in *address) {
	const int socket_id = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (socket_id < 0) {
		perror("socket");
		exit(1);
	}

	unsigned char buf[PACKETSIZE * sizeof(uint16_t)];
	unsigned int address_len = sizeof(*address);

	const int n = recvfrom(socket_id, buf, sizeof(buf), 0, (struct sockaddr *)address, &address_len);

	if (n == -1) {
		perror("recvfrom");
		exit(1);
	}

	char address_string[INET6_ADDRSTRLEN];
	const char *ret = inet_ntop(address->sin_family, &(address->sin_addr), address_string, INET6_ADDRSTRLEN);
	if (ret == NULL) {
		perror("inet_ntop");
		exit(1);
	}

	fprintf(stderr, "connected from %s\n", address_string);

	return;
}

int main(int argc, char const *argv[]) {
	int ret, i, j;

	if (argc >= 3) {
		const char *usage =
			"Usage: %s                 (for host)\n"
			"       %s <host address>  (for client)\n";
		fprintf(stderr, usage, argv[0], argv[0]);
		exit(1);
	}

	const enum Mode mode = (argc == 2) ? MODE_CLIENT : MODE_HOST;

	struct sockaddr_in address;
	if (mode == MODE_CLIENT) {
		address.sin_family = AF_INET;
		address.sin_port = 0; // No "port" for ICMP. So this is dummy.

		ret = inet_aton(argv[1], &(address.sin_addr));
		if (ret == 0) {
			perror("inet_aton");
			exit(1);
		}

		// Start of connection
		const uint8_t data[1] = {0};
		ping(&address, data, sizeof(data), 0);
	} else {
		assert(mode == MODE_HOST);
		await_connection(&address);
	}

	OpusEncoder *encoder = opus_encoder_create(SAMPLE_RATE, 1, OPUS_APPLICATION_AUDIO, &ret);
	if (ret != OPUS_OK) {
		perror("opus_encoder_create");
		exit(1);
	}

	OpusDecoder *decoder = opus_decoder_create(SAMPLE_RATE, 1, &ret);
	if (ret != OPUS_OK) {
		perror("opus_decoder_create");
		exit(1);
	}

	ret = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(BITRATE));
	if (ret < 0) {
		perror("opus_encoder_ctl");
		exit(1);
	}

	if (decoder != NULL) {
	}

	uint16_t send_sequence_number = 1;
	uint16_t receive_sequence_number = 0;

	while (1) {
		// Send

		uint8_t packet_data[MAX_FRAME_SIZE * FRAMES_PER_PACKET];
		int packet_data_size = 0;

		for (i = 0; i < FRAMES_PER_PACKET; i++) {
			uint8_t buffer[SAMPLES_PER_FRAME * sizeof(opus_int16)];
			opus_int16 input_data[SAMPLES_PER_FRAME];

			ret = read_n(0, SAMPLES_PER_FRAME * sizeof(opus_int16), buffer);

			if (ret == 0) {
				exit(0);
			}

			// BE => LE
			for (j = 0; j < SAMPLES_PER_FRAME; j++) {
				input_data[j] = buffer[2 * j + 1] << 8 | buffer[2 * j];
			}

			unsigned char output_data[MAX_FRAME_SIZE];
			opus_int32 output_data_length = opus_encode(encoder, input_data, SAMPLES_PER_FRAME, output_data, MAX_FRAME_SIZE);
			if (output_data_length < 0) {
				perror("opus_encode");
				exit(1);
			}

			assert(output_data_length < MAX_FRAME_SIZE);

			packet_data[packet_data_size] = (uint8_t)output_data_length;
			memcpy(packet_data + packet_data_size + 1, output_data, output_data_length);

			packet_data_size += output_data_length + 1;
		}

		fprintf(stderr, "Ping: %d bytes, Seq = %d\n", packet_data_size, send_sequence_number);

		ping(&address, packet_data, packet_data_size, send_sequence_number);
		send_sequence_number++;

		// Receive

		const int socket_id = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (socket_id < 0) {
			perror("socket");
			exit(1);
		}

		unsigned char buf[PACKETSIZE * sizeof(uint16_t)];
		unsigned int address_len = sizeof(address);
		const int n = recvfrom(socket_id, buf, sizeof(buf), 0, (struct sockaddr *)&address, &address_len);
		if (n == -1) {
			perror("recvfrom");
			exit(1);
		}

		struct iphdr *ip_header = (struct iphdr*)buf;
		struct icmphdr *icmp_header = (struct icmphdr*)(buf + ip_header->ihl * 4);
		uint8_t *icmp_body = buf + ip_header->ihl * 4 + sizeof(icmp_header);

		receive_sequence_number = icmp_header->un.echo.sequence;

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

		usleep(200 * 1000);
	}

	return 0;
}
