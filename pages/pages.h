#ifndef pages_h
#define pages_h

#include <time.h>
#include <unistd.h>

#include "../lib/strings.h"
#include "../lib/strings.c"
#include "../lib/screens.h"
#include "../lib/screens.c"
#include "../lib/files.h"
#include "../lib/files.c"

#define pages_word_len 16
#define pages_definition_len 16
#define pages_date_len 10
#define pages_score_len 16
#define pages_note_len 256
#define pages_line_len 319

#define pages_definition_offset(i) (pages_line_len * i) + pages_word_len
#define pages_date_offset(i) pages_definition_offset(i) + pages_definition_len + 1
#define pages_score_offset(i) pages_date_offset(i) + pages_date_len + 1
#define pages_note_offset(i) pages_score_offset(i) + pages_score_len + 1
#define pages_line_offset(i) pages_note_offset(i) + pages_note_len + 1

#define archives_filename "./state/archive.csv"

const string colsep = {.text="=", .size=2};
const string linesep = {.text="\n", .size=2};

void menus_print(void);

__attribute_warn_unused_result__
vector pages_make(const char *const filename, int mode);

__attribute_warn_unused_result__
string pages_make_line(const string *word);

void pages_search(string *buffer);

void pages_filter(const vector *page_lines, const string *word, vector *page_filtered);

#endif
