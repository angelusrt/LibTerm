#ifndef vectors_h
#define vectors_h
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <limits.h>
#include "errors.c"

#define vectors_min 16

typedef enum _vector_types {
	vectors_string_type,
	vectors_vector_type,
	vectors_size_type,
	vectors_double_type,
	vectors_float_type,
	vectors_string_virtual_type,
} vectors_type;

typedef struct _vector_struct {
	vectors_type type;
	size_t size;
	size_t capacity;
	void *data;
} vector;

// hashs<vector<size_t>>, freqs<vector<size_t>>, itens<vector<void *>>
typedef struct _set_struct {
	vector hashs; //size_t
	vector freqs; //size_t
	vector itens; //void *
} set;

typedef struct _vector_stats {
	double min;
	double max;
	double mean;
	double total;
} vector_stats;

#include "errors.h"
#include "strings.h"

typedef bool (*compfunc)(void *, void *);
typedef bool (*sets_compfunc)(set *, size_t);
typedef size_t (*hashfunc)(void *);
typedef void (*cleanfunc)(void *);


__attribute_warn_unused_result__
vector vectors_init(vectors_type type);

//returns true if it isn't valid else 0
__attribute_warn_unused_result__
bool vectors_check(const vector *v);

bool vectors_push(vector *v, void *item);

bool vectors_pop(vector *v);

__attribute_warn_unused_result__
vector_stats vectors_stat(vector *v);

/*insertion sort*/
void vectors_sort(vector *v, compfunc compare);

void vectors_free(vector *v);

void vectors_normalize(vector *v);


__attribute_warn_unused_result__
set sets_init(vectors_type type);

//returns true if it isn't valid else 0
__attribute_warn_unused_result__
bool sets_check(const set *s);

// true if already exists else false
bool sets_push(set *s, void *item);

void sets_sort_freqs(set *s);

void sets_sort_hashs(set *s);

__attribute_warn_unused_result__
size_t sets_freqs_biggest(const set *s);

__attribute_warn_unused_result__
size_t sets_freqs_total(const set *s);

__attribute_warn_unused_result__
long sets_find_hash(const set *s, size_t hash);

__attribute_warn_unused_result__
long sets_bfind_hash(const set *s, size_t hash);

void sets_free(set *s);

#endif
