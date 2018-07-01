/*
 *  org2dat: No previous attribution notice.
 *  Original author appears to be Vincent Spader,
 *   according to "swap.h" which mentions this project.
 *  Modified by 20kdc on 6/30/18.
 */
#include <stdio.h>
#include <stdlib.h>

#include "organya.h"

int main (int argc, const char * argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <org_file>\n", argv[0]);
		return 1;
	}
	
	const char *path = argv[1];
	
	org_decoder_t *decoder = org_decoder_create(path, "samples", 3);
	if (!decoder)
		return 1;
	
	fprintf(stderr, "total samples: %zu\n", org_decoder_get_total_samples(decoder));
	size_t num_samples = 44100*100;
	size_t samples_read = 0;
	int16_t *buffer = malloc(num_samples*2*sizeof(int16_t));
	do {
		samples_read = org_decode_samples(decoder, buffer, num_samples);
		fwrite(buffer, 2*2, samples_read, stdout);
	} while (samples_read == num_samples);
	
	free(buffer);
	org_decoder_destroy(decoder);
	
    return 0;
}
