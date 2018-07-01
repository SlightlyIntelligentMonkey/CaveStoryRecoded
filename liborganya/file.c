/*
 *  file.c
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
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "swap.h"

// File reading helpers 
uint8_t _org_read_8(FILE *fin) {
	uint8_t i = 0;
	fread(&i, sizeof(i), 1, fin);
	return i;
}

uint16_t _org_read_16(FILE *fin) {
	uint16_t i = 0;
	fread(&i, sizeof(i), 1, fin);
	return org_ltoh_16(i);
}

uint32_t _org_read_32(FILE *fin) {
	uint32_t i = 0;
	fread(&i, sizeof(i), 1, fin);
	return org_ltoh_32(i);
}

// Read the usual org header
uint8_t _org_read_header(org_header_t *header, FILE *fin)
{
	// Read the magic. All orgyana files start with Org-02.
	int8_t buf[6];
	fread(buf, 6, 1, fin);
	if(0 != memcmp(buf, "Org-02", 6)) {
		return 0;
	}
	
	header->tempo = _org_read_16(fin);
	header->steps_per_bar = _org_read_8(fin);
	header->beats_per_step = _org_read_8(fin);
	header->loop_start = _org_read_32(fin);
	header->loop_end = _org_read_32(fin);
	
	return 1;
}

// Read properties for the instrument
void _org_read_instrument(org_instrument_t *instrument, FILE *fin)
{
	instrument->pitch = _org_read_16(fin);
	instrument->instrument = _org_read_8(fin);
	instrument->disable_sustain = _org_read_8(fin);
	instrument->note_count = _org_read_16(fin);
}

// Read properties for each note
void _org_read_notes(org_note_t notes[], FILE *fin, uint16_t note_count)
{
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].start = _org_read_32(fin);
	}
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].key = _org_read_8(fin);
	}
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].length = _org_read_8(fin);
	}
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].volume = _org_read_8(fin);
	}
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].pan = _org_read_8(fin);
	}
}

// Rather straightforward just follows the file format.
org_file_t *_org_file_create(const char *filename) {
	FILE *fin = fopen(filename, "rb");
	if (NULL == fin) {
		// Could not open the file.
		return NULL;
	}

	org_file_t *org = (org_file_t *) malloc(sizeof(org_file_t));
	uint8_t success = _org_read_header(&org->header, fin);
	if (!success) {
		// There was an error. Clean up.
		fclose(fin);
		free(org);

		return NULL;
	}
	
	// Read instrument properties
	for (uint8_t i = 0; i < 16; i++) {
		_org_read_instrument(&org->instruments[i], fin);
		
		// Allocate space for notes
		if (org->instruments[i].note_count) {
			org->instruments[i].notes = (org_note_t *) malloc(sizeof(org_note_t) * org->instruments[i].note_count);
		}
		else {
			org->instruments[i].notes = NULL;
		}
	}
	
	// Read notes for each instrument
	for (uint8_t i = 0; i < 16; i++) {
		_org_read_notes(org->instruments[i].notes, fin, org->instruments[i].note_count);
	}
	
	// Close the ifle
	fclose(fin);
	
	return org;
}

void _org_file_destroy(org_file_t *org) {
	// Free up memory
	for (uint8_t i = 0; i < 16; i++) {
		if (org->instruments[i].notes) free(org->instruments[i].notes);
	}
	
	free(org);
}
