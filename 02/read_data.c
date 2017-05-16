#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	const char *filename = argv[1];

	FILE *file = fopen(filename, "rb");

	if (file == NULL) {
		perror("fopen");
		exit(1);
	}

	int character;
	int i = 0;
	while ((character = fgetc(file)) != EOF) {
		printf("%d %d\n", i, character);
		i++;
	}

	fclose(file);

	return 0;
}
