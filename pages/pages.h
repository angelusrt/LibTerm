#ifndef pages_h
#define pages_h

#include <time.h>

#include "../lib/strings.h"
#include "../lib/strings.c"
#include "../lib/screens.h"
#include "../lib/screens.c"
#include "../lib/files.h"
#include "../lib/files.c"

#define pages_word_len 16
#define pages_score_len 16
#define pages_date_len 10
#define pages_note_len 256
#define pages_line_len 302
#define pages_note_offset pages_word_len + 1 + pages_date_len + 1 + pages_score_len + 1

#define archives_filename "./state/archive.csv"

const string colsep = {.text="=", .size=2};
const string linesep = {.text="\n", .size=2};

void menus_print(void);

__attribute_warn_unused_result__
vector pages_make(const char *const filename, int mode);

__attribute_warn_unused_result__
string pages_make_line(const string *word);

#endif
