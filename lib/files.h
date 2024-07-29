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
int files_make(char *filename, int mode);

//It returns 1 or 0; "1" meaning it ended or error'd; "0" meaning it is able to continue.
__attribute_warn_unused_result__
bool files_read(int f, string *buffer);

//It returns 1 for error or 0 for success
__attribute_warn_unused_result__
bool files_sync(int f, string *buffer, const string *sep, size_t index);

#endif
