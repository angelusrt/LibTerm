#ifndef errors_h
#define errors_h

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define colors_error(text) "\033[31m" text "\033[0m\n"
#define colors_warn(text) "\033[33m" text "\033[0m\n"
#define colors_success(text) "\033[32m" text "\033[0m\n"

__attribute_warn_unused_result__
bool errors_assert(char *message, bool statement);

void errors_panic(char *message, bool statement);

bool errors_warn(char *message, bool statement);

#endif
