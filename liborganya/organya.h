/*
 *  organya.h
 *  liborganya
 *
 *  Created by Vincent Spader on 6/20/09.
 *  Modified by 20kdc on 6/30/18.
 *  (I assume the library README's Public Domain statement
 *    takes precedence over the IDE-generated,
 *    and thus not written by the author,
 *    all-rights-reserved copyright notice
 *    that was here.
 *   Should I be wrong, please ask to have the library removed. - 20kdc)
 *
 */

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" { 
#endif

typedef struct org_decoder_t org_decoder_t;

// org_path: the path to the .org file
// resource_path: the path to a directory containing the sample data files
// loop_count: number of times to loop the file. pass 0 to loop forever.
// returns NULL on error, otherwise a decoder to use with other functions
org_decoder_t *org_decoder_create(const char *org_path, const char *resource_path, uint32_t loop_count);

// buffer: where the decoder will put the generated samples.
// num_samples: the number of frames to decode. does not include channels or bits per sample.
// returns the number of samples actually decoded.
// data returned in buffer is signed 16-bit 44100hz in host endian.
size_t org_decode_samples(org_decoder_t *decoder, int16_t *buffer, size_t num_samples);

// Returns the total number of samples a decoder will read. Basically, the length of the song. Returns 0 if the decoder is set to loop indefinitely.
size_t org_decoder_get_total_samples(org_decoder_t *decoder);

// Seek to the desired sample
void org_decoder_seek_sample(org_decoder_t *decoder, size_t sample);

// clean up the decoder
void org_decoder_destroy(org_decoder_t *decoder);

#ifdef __cplusplus
}
#endif
		
