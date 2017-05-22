#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <sample_rate>\n", argv[0]);
		exit(1);
	}

	const int sample_rate = atoi(argv[1]);

	int16_t value[0];
	int i = 0;
	while (fread(value, sizeof(int16_t), 1, stdin) == 1) {
		if (i % sample_rate == 0) {
			fwrite(value, sizeof(int16_t), 1, stdout);
		}
		i++;
	}

	return 0;
}
