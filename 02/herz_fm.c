#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SAMPLING_FREQUENCY 44100

const char herz[256] = " cdegffAggCBCgecdefgAgfedecbcd6bdfedecdegffAggCBCgecdeagfedc6cbcegCgecegCCCCCC";
const char herzbase[256] = "2634063456162054065512222";

int get_height(char note) {
	int note_height;

	if ('a' <= note && note <= 'g') {
		switch (note - 'a') {
			case 0: note_height = 1; break;
			case 1: note_height = 3; break;
			case 2: note_height = 4; break;
			case 3: note_height = 6; break;
			case 4: note_height = 8; break;
			case 5: note_height = 9; break;
			case 6: note_height = 11; break;
		}
	} else if ('A' <= note && note <= 'G') {
		switch (note - 'A') {
			case 0: note_height = 1; break;
			case 1: note_height = 3; break;
			case 2: note_height = 4; break;
			case 3: note_height = 6; break;
			case 4: note_height = 8; break;
			case 5: note_height = 9; break;
			case 6: note_height = 11; break;
		}

		note_height += 12;
	} else if ('0' <= note && note <= '6') {
		switch (note - '0') {
			case 0: note_height = 1; break;
			case 1: note_height = 3; break;
			case 2: note_height = 4; break;
			case 3: note_height = 6; break;
			case 4: note_height = 8; break;
			case 5: note_height = 9; break;
			case 6: note_height = 11; break;
		}

		note_height -= 12;
	} else {
		return -100;
	}

	return note_height - 24;
}

int main(int argc, char const *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <amplitude> <samples>\n", argv[0]);
		exit(1);
	}

	const int amplitude = atoi(argv[1]);
	const unsigned int samples = atoi(argv[2]);

	int note_index;
	for (note_index = 0; note_index < strlen(herz); note_index++) {
		const char note = herz[note_index];
		const char base = herzbase[(int)(note_index / 3)];
		const int note_height = get_height(note);
		const int base_height = get_height(base);
		unsigned int i;
		short buffer;

		for (i = 0; i < samples; i++) {
			if (note_height == -100) {
				buffer = 0;
			} else {
				const double frequency = 440.0 * pow(2.0, (note_height - 1) / 12.0);
				const double modulation_index = 10.0;
				const double modulator_ratio = 2.0;
				buffer = amplitude * sin(
					2.0 * (double)M_PI * frequency
					* (double)(note_index * samples + i) / (double)SAMPLING_FREQUENCY
					+ modulation_index * sin(
						2.0 * (double)M_PI * frequency * modulator_ratio
						* (double)(note_index * samples + i) / (double)SAMPLING_FREQUENCY
					)
				);
			}

			if (1 || base_height == -100) {
				buffer += 0;
			} else {
				const double frequency = 440.0 * pow(2.0, (base_height - 1) / 12.0);
				buffer += amplitude * sin(2.0 * (double)M_PI * frequency * (double)(note_index * samples + i) / (double)SAMPLING_FREQUENCY);
			}

			int written_bytes = fwrite(&buffer, sizeof(short), 1, stdout);

			if (written_bytes < 1) {
				perror("fwrite");
				exit(1);
			}
		}
	}

	return 0;
}
