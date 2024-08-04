#ifndef notes_h
#define notes_h

#include "pages.h"
#include "../lib/strings.h"
#include "../lib/strings.c"
#include "../lib/errors.h"
#include "../lib/errors.c"
#include "../lib/screens.h"
#include "../lib/screens.c"

typedef enum _notes_status {
	notes_note_inserted_status = 1,
	notes_note_not_inserted_status,
	notes_note_removed_status,
	notes_note_not_removed_status,
} notes_status;

#define notes_filename "./state/apprentice.csv"

void notes_print(const string *line, size_t current, size_t total, notes_status notes_stat);

// -1 if note wasn't found, an index otherwise
long notes_find(const vector *note_lines, const string *entry, string_virtual *note);

//Return 0 if error else 1 if successfull
__attribute_warn_unused_result__
bool notes_add(int file, const string *entry);

// It returns 0 for inserting conditionally, 1 for successfully inserting, or -1 for errors
__attribute_warn_unused_result__
int notes_insert(int file, size_t index);

//It returns 1 for successfully removing it else 0
__attribute_warn_unused_result__
bool notes_remove(int file, size_t index, size_t last, string_virtual *note);

void notes_update(vector *note_lines, string **notes);

#endif
