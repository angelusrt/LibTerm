#ifndef utils_h
#define utils_h

#include "../lib/strings.h"
#include "../lib/strings.c"
#include "../lib/files.h"
#include "../lib/files.c"

__attribute_warn_unused_result__
long utils_find_line(int file, string *word, size_t bytes);

void utils_print_frequency(string_virtual *cols_data);

#endif
