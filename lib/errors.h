#ifndef errors_h
#define errors_h

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#define colors_error(text) "\033[31m" text "\033[0m\n"
#define colors_warn(text) "\033[33m" text "\033[0m\n"
#define colors_success(text) "\033[32m" text "\033[0m\n"

#define errors_debug_mode true

__attribute_warn_unused_result__
bool errors_assert(const char *message, bool statement);

void errors_panic(const char *message, bool statement);

bool errors_warn(const char *message, bool statement);

__attribute_warn_unused_result__
bool errors_check(const char *message, bool statement);

__attribute__ ((__format__ (printf, 1, 2)))
void errors_debug(const char *const message, ...);

#endif
