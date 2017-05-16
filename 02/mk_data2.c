#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	const char *filename = argv[1];

	FILE *file = fopen(filename, "wb");

	if (file == NULL) {
		perror("fopen");
		exit(1);
	}

	unsigned int i;
	for (i = 0; i < 0x100; i++) {
		int written_bytes = fwrite((char *)&i, sizeof(char), 1, file);

		if (written_bytes < 1) {
			perror("fwrite");
			exit(1);
		}
	}

	fclose(file);

	return 0;
}
