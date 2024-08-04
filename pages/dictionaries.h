#ifndef dictionaries_h
#define dictionaries_h

#include "pages.h"
#include "../lib/strings.h"
#include "../lib/strings.c"
#include "../lib/errors.h"
#include "../lib/errors.c"
#include "../lib/screens.h"
#include "../lib/screens.c"

typedef enum dictionaries_status {
    dictionaries_note_added_status = 1,
    dictionaries_note_not_added_status,
    dictionaries_note_exists_status,
    dictionaries_note_inserted_status,
    dictionaries_note_not_inserted_status,
} dictionaries_status;

//note can be of size 0
void dictionaries_print(
	const string *line, size_t current, size_t total, const string_virtual *note, dictionaries_status dict_stat
);

#endif
