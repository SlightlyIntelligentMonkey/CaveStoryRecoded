/*
 *  file.h
 *  liborganya
 *
 *  Created by Vincent Spader on 6/20/09.
 *  Modified by 20kdc on 6/30/18.
 *  Modified by Cucky on 7/01/18.
 *
 *  (I assume the library README's Public Domain statement
 *    takes precedence over the IDE-generated,
 *    and thus not written by the author,
 *    all-rights-reserved copyright notice
 *    that was here.
 *   Should I be wrong, please ask to have the library removed. - 20kdc)
 *
 */

#include <stdint.h>
#include <stdio.h>

#pragma pack(push, 1)

typedef struct {
	uint16_t  tempo; // milliseconds per beat
	uint8_t steps_per_bar; // not used
	uint8_t beats_per_step; // not used
	uint32_t loop_start; // in beats
	uint32_t loop_end; // in beats
} org_header_t;

typedef struct {
	uint32_t start; // The beat the note is on
	uint8_t key; // A value representing what pitch the note is. Interpretation varies based on the instrument.
	uint8_t length; // how many beats the note should play for
	uint8_t volume;
	uint8_t pan;
} org_note_t;

typedef struct {
	uint16_t pitch; // Affects the frequency the instrument will be played at.
	uint8_t instrument; // melody 0-99, drums 0-11. The first 8 instruments are melody, the last 8 are drums
	uint8_t disable_sustain; // 0 if the sample for the instrument should repeat. Otherwise, it will not repeat.
	uint16_t note_count; // total number of notes the instrument has
	
	org_note_t *notes; // An array of notes
} org_instrument_t;

typedef struct {
	org_header_t header;
	org_instrument_t instruments[16];
} org_file_t;

#pragma pack(pop)

// File reading helpers
uint8_t _org_read_8(FILE *fin);
uint16_t _org_read_16(FILE *fin);
uint32_t _org_read_32(FILE *fin);

uint8_t _org_read_header(org_header_t *header, FILE *fin); // Reads header data from a file. Returns 1 on success, 0 on fail
void _org_read_instrument(org_instrument_t *instrument, FILE *fin); // Reads instrument data from the file
void _org_read_notes(org_note_t notes[], FILE *fin, uint16_t note_count); // Reads note data from the file

// Used by the decoder. Creates a org_file_t struct and parses the file passed in. Returns NULL on error.
org_file_t *_org_file_create(const char *filename);
// Cleans up
void _org_file_destroy(org_file_t *org);
