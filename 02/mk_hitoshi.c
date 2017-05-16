#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
	unsigned char bytes[6] = {228, 186, 186, 229, 191, 151};

	FILE *file = fopen("hitoshi.bin", "wb");

	if (file == NULL) {
		perror("fopen");
		exit(1);
	}

	unsigned int i = 0;
	for (i = 0; i < sizeof(bytes) / sizeof(unsigned char); i++) {
		fwrite(bytes + sizeof(unsigned char) * i, sizeof(unsigned char), 1, file);
	}

	fclose(file);

	return 0;
}
