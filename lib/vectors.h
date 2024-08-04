#ifndef vectors_h
#define vectors_h
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/cdefs.h>

#define vectors_min 16

typedef enum _vector_types {
	vectors_string_type,
	vectors_vector_type,
	vectors_size_type,
	vectors_string_virtual_type,
} vectors_type;

typedef struct _vector_struct {
	vectors_type type;
	size_t size;
	size_t capacity;
	void *data;
} vector;

typedef struct _set_struct {
	vector hashs; //size_t
	vector freqs; //size_t
	vector itens; //void *
} set;

#include "errors.h"
#include "strings.h"

typedef bool (*compfunc)(void *, void *);
typedef size_t (*hashfunc)(void *);
typedef void (*cleanfunc)(void *);


__attribute_warn_unused_result__
size_t vectors_types_size(vectors_type type);

void vectors_types_insert(vectors_type type, void *data, void *item, size_t index);

void vectors_types_cleanup(vectors_type type, void *data, size_t index);


__attribute_warn_unused_result__
vector vectors_init(vectors_type type);

__attribute_warn_unused_result__
bool vectors_check(const vector *v);

bool vectors_push(vector *v, void *item);

bool vectors_pop(vector *v);

/*insertion sort*/
void vectors_sort(vector *v, compfunc compare);

void vectors_free(vector *v);

#endif
