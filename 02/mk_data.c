#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
	FILE *file = fopen("my_data.bin", "wb");

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
