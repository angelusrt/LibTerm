#ifndef files_h
#define files_h
#pragma once

#include <sys/cdefs.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "errors.h"
#include "strings.h"
#include "vectors.h"

__attribute_warn_unused_result__
int files_make(const char *filename, int mode);

//It returns -1 if error, 0 if it ended else 1 if it didn't ended.
__attribute_warn_unused_result__
int files_read(int f, string *buffer);

__attribute_warn_unused_result__
set files_make_trim_unique(const char *filename, int mode, const string *seps);

//It returns 1 for error or 0 for success
__attribute_warn_unused_result__
bool files_sync(int f, string *buffer, const string *sep, size_t index);

#endif
