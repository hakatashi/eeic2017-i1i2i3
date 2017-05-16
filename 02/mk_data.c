#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>", argv[0]);
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
		fwrite((char *)&i, sizeof(char), 1, file);
	}

	fclose(file);

	return 0;
}
