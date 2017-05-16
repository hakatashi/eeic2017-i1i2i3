#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SAMPLING_FREQUENCY 44100

int main(int argc, char const *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s <amplitude> <frequency> <samples>\n", argv[0]);
		exit(1);
	}

	const int amplitude = atoi(argv[1]);
	const unsigned int frequency = atoi(argv[2]);
	const unsigned int samples = atoi(argv[3]);

	unsigned int i;
	short buffer;
	for (i = 0; i < samples; i++) {
		buffer = amplitude * sin(2.0 * (double)M_PI * (double)frequency * (double)i / (double)SAMPLING_FREQUENCY);
		int written_bytes = fwrite(&buffer, sizeof(short), 1, stdout);

		if (written_bytes < 1) {
			perror("fwrite");
			exit(1);
		}
	}

	return 0;
}
