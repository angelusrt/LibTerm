#ifndef pages_h
#define pages_h

#include <sys/cdefs.h>
#include <unistd.h>
#include <time.h>

#include "../lib/strings.h"
#include "../lib/strings.c"

typedef enum _page_type_struct {
	pages_dictionary_type,
	pages_notes_type,
	pages_menu_type,
} page_type;

typedef struct _page_struct {
	int file;

	string batch;
	short batch_curr;
	ushort batch_last;

	vector lines;
	short line_curr;
	ushort line_last;

	vector pages;
	page_type page_type;
} page;

#define pages_word_len 16
#define pages_score_len 16
#define pages_date_len 10
#define pages_note_len 256
#define pages_line_len 302

void pages_find_columns(page *p, vector *cols);

void pages_free(page *p);

__attribute_warn_unused_result__
string pages_make_line(string *word);

void pages_print_header(const page *p, const string *s);

__attribute_warn_unused_result__
string pages_log(const page_type p);

#endif
