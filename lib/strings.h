#ifndef strings_h
#define strings_h
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>

#include "errors.h"
#include "errors.c"
#include "vectors.h"

#define hash_const 7

typedef struct _string_virtual_struct {
	size_t size;
	char *text;
} string_virtual;

typedef struct _string_struct {
	size_t size;
	char *text;
	size_t capacity;
} string;

typedef enum _strings_size {
	strings_min = 16,
	strings_small = 1024,
	strings_big = 4096,
	strings_large = 65536,
	strings_very_large = 1048576, 
	strings_max = SIZE_MAX,
} strings_size;

#define strings_init_buffer(buffer, length) \
	char buf_##buffer[length] = ""; \
	buffer = (string) {.text=buf_##buffer, .size=length, .capacity=length}

__attribute_warn_unused_result__
string strings_init(strings_size size);

__attribute_warn_unused_result__
string strings_lit(char *const lit);

__attribute_warn_unused_result__
string strings_make(const char *lit);

//returns true if it failed to resize
__attribute_warn_unused_result__
bool strings_upgrade(string *s);

void strings_free(string *s);

void strings_free_many(size_t n, string *s, ...);

//returns true if first is bigger else false
__attribute_warn_unused_result__
bool strings_compare(const string *first, const string *second);

__attribute_warn_unused_result__
bool strings_check(const string *s);

__attribute_warn_unused_result__
bool strings_check_extra(const string *s);

__attribute_warn_unused_result__
bool string_virtuals_check(const string_virtual *s);

//returns vector<size_t>
__attribute_warn_unused_result__
vector strings_find(const string *s, const string *sep);

void strings_replace(string *s, const string *seps, const char rep[1]);

__attribute_warn_unused_result__
string strings_make_replace(const string *s, const string *sep, const string *rep);

__attribute_warn_unused_result__
vector strings_trim(const string *s, const string *sep);

__attribute_warn_unused_result__
set strings_trim_unique(const string *s);

__attribute_warn_unused_result__
__attribute__ ((__format__ (printf, 1, 2)))
string strings_make_format(const char *const form, ...);

void strings_trim_virtual(const string *s, const string *sep, vector *sentences);

__attribute_warn_unused_result__
size_t strings_hasherize(const string *s);

void strings_print(const string *s);

void strings_print_no_panic(const string *s);

void strings_println(const string *s);

#endif
