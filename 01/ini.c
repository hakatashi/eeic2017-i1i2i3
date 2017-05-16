#include <stdio.h>

int main(int argc, char const *argv[]) {
	const char *gn = argv[1];
	const char *fn = argv[2];

	const char g = gn[0];
	const char f = fn[0];

	printf("initial of '%s %s' is %c%c.\n", gn, fn, g, f);

	return 0;
}
