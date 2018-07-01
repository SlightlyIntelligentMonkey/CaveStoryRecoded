/*
 *  decoder.h
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

#include "file.h"

typedef struct {
	uint16_t current_note; // Index into org_instrument_t.notes
	uint8_t playing; // Whether or not the track is currently playing
	
	org_note_t note; // Current note properties.
} org_track_t;

typedef struct {
	int16_t *wave; // PCM data. samples are unsigned 16-bit BIG ENDIAN at 22050hz on disk. Converted to host endian when loaded.
	uint32_t length; // number of samples
} org_sample_data_t;

typedef struct {
	uint8_t primed; // Simple flag to determine if the decoder has been used or not
	uint32_t current_beat; // The current beat number
	uint32_t current_sample; // The current sample number

	uint32_t current_loop; // The current loop number
	uint32_t loop_count; // The total number of times we will loop. This is passed in org_decoder_create. 
	
	org_track_t tracks[16]; // 16 tracks for each org file
} org_decoder_state_t;

typedef struct {
	org_file_t *file;
	org_decoder_state_t state;
	org_sample_data_t samples[16]; // The sample data for each track
} org_decoder_t;

uint8_t _org_decoder_load_samples(org_decoder_t *decoder, const char *resource_path); // Loads instrument sample data for the decoder
void _org_advance_beat(org_decoder_t *decoder); // Advances the decoder by 1 beat
void org_decoder_destroy(org_decoder_t *decoder); // Clean up the decoder
