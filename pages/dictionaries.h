#ifndef dictionaries_h
#define dictionaries_h

#include <string.h>

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
	dictionaries_note_removed_status,
	dictionaries_note_not_removed_status,
    dictionaries_note_defined_status,
    dictionaries_note_not_defined_status,
    dictionaries_filter_enabled_status,
    dictionaries_filter_disabled_status,
    dictionaries_filter_not_matched_status,
} dictionaries_status;

typedef enum dictionaries_sorting {
	dictionaries_not_sort,
	dictionaries_word_sort,
	dictionaries_frequency_sort,
} dictionaries_sorting;

#define dictionaries_filename "./state/dictionary.csv"
#define max_freq 5

void dictionaries_print(
	const string *line, size_t current, size_t total, 
	dictionaries_status dict_stat, dictionaries_sorting dict_sort
);

void dictionaries_print_note(const string *note);

//returns false if first frequency is bigger than the second else true
__attribute_warn_unused_result__
bool dictionaries_compare_frequency(string *first, string *second);

#endif
