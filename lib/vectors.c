#ifndef vectors_c
#define vectors_c
#pragma once

#include "vectors.h"
#include "strings.c"

size_t vectors_types_size(vectors_type type) {
    switch (type) {
    case vectors_string_type:
        return sizeof(string);
    case vectors_vector_type:
        return sizeof(vector);
    case vectors_size_type:
        return sizeof(size_t);
    case vectors_string_virtual_type:
        return sizeof(string_virtual);
    }

    return sizeof(size_t);
}

/*
void vectors_types_insert(vectors_type type, void *data, void *item, size_t index) {
    switch (type) {
    case vectors_string_type:
        ((string *)data)[index] = *(string *)item;
        return;
    case vectors_vector_type:
        ((vector *)data)[index] = *(vector *)item;
        return;
    case vectors_size_type:
        ((size_t *)data)[index] = (size_t)item;
        return;
    case vectors_string_virtual_type:
        ((string_virtual *)data)[index] = *(string_virtual *)item;
        return;
    }

    ((size_t *)data)[index] = (size_t)item;
    return;
}
*/

void vectors_types_insert(vectors_type type, void *data, size_t data_index, void *item, size_t item_index) {
    switch (type) {
    case vectors_string_type:
        ((string *)data)[data_index] = ((string *)item)[item_index];
        return;
    case vectors_vector_type:
        ((vector *)data)[data_index] = ((vector *)item)[item_index];
        return;
    case vectors_size_type:
		((size_t *)data)[data_index] = (size_t)item;
        return;
    case vectors_string_virtual_type:
        ((string_virtual *)data)[data_index] = ((string_virtual *)item)[item_index];
        return;
    }

    ((size_t *)data)[data_index] = (size_t)item;
    return;
}

void *vectors_types_get(vectors_type type, void *item, size_t item_index) {
    switch (type) {
    case vectors_string_type:
        return (((string *)item) + item_index);
    case vectors_vector_type:
        return (((vector *)item) + item_index);
    case vectors_size_type:
        return (void *)((size_t)item);
    case vectors_string_virtual_type:
        return (((string_virtual *)item) + item_index);
    }

	return (void *)((size_t)item);
}

void vectors_types_cleanup(vectors_type type, void *data, size_t index) {
    switch (type) {
    case vectors_string_type:
        strings_free(&((string *)data)[index]);
        return;
    case vectors_vector_type:
        vectors_free(&((vector *)data)[index]);
        return;
    case vectors_size_type:
    case vectors_string_virtual_type:
        return;
    }

    return;
}

vector vectors_init(vectors_type type) {
    size_t type_size = vectors_types_size(type);
    void *data = malloc(vectors_min * type_size);
    errors_panic("vectors_init (data)", data == NULL);

    return (vector){
        .size = 0, .capacity = vectors_min, .type = type, .data = data};
}

bool vectors_check(const vector *v) {
    if (errors_check("vectors_check (v)", v == NULL)) return true;
    if (errors_check("vectors_check (v.data)", v->data == NULL)) return true;
    if (errors_check("vectors_check (v.capacity < vectors_min)", v->capacity < vectors_min)) return true;

    return false;
}

bool vectors_push(vector *v, void *item) {
    errors_panic("vectors_push (v veio nulo)", v == NULL);
    errors_warn("vectors_push (item veio 0)", item == 0);

    v->size++;
    if (v->size > v->capacity - 1) {
        v->capacity = v->capacity << 1;

        size_t type_size = vectors_types_size(v->type);
        void *new_data = realloc(v->data, (v->capacity) * type_size);

        if (new_data == NULL) {
            printf(colors_warn("vectors_push (new_data == NULL)"));
            return true;
        }

        v->data = new_data;
    }

    vectors_types_insert(v->type, v->data, v->size - 1, item, 0);

    return false;
}

bool vectors_pop(vector *v) {
    errors_panic("vectors_pop (v veio nulo)", v == NULL);

    if (v->size == 0) {
        printf(colors_warn("vectors_pop (v.size veio 0)"));
        return true;
    }

    vectors_types_cleanup(v->type, v->data, v->size - 1);
    v->size--;

    if (v->size < (v->capacity >> 1)) {
        v->capacity = v->capacity >> 1;

        size_t type_size = vectors_types_size(v->type);
        void *new_v = realloc(v->data, v->capacity * type_size);

        if (new_v == NULL) {
            printf(colors_warn("vectors_pop (new_v veio nulo)"));
            return true;
        }

        v->data = new_v;
    }

    return false;
}

 void vectors_sort(vector *v, compfunc compare) {
	errors_panic("vectors_sort (v)", vectors_check(v));

	if (v->size <= 1) {
		printf(colors_warn("vectors_sort (v.size <= 1)"));
		return;
	}

	for (size_t i = v->size; i > 0; i--) {
		for (size_t j = 1; j < i; j++) {
			void **prev_data = vectors_types_get(v->type, v->data, j - 1); 				
			void **curr_data = vectors_types_get(v->type, v->data, j); 				

			bool is_bigger = compare(prev_data, curr_data);
			if (is_bigger) {
				vectors_types_insert(v->type, v->data, v->capacity - 1, v->data, j); 				
				vectors_types_insert(v->type, v->data, j, v->data, j - 1);
				vectors_types_insert(v->type, v->data, j - 1, v->data, v->capacity - 1);
			}
		}
	}
 }

void vectors_free(vector *v) {
    if (v == NULL) {
        printf(colors_warn("vectors_free (v == NULL)"));
        return;
    }

    while (v->size > 0) {
        vectors_types_cleanup(v->type, v->data, v->size - 1);
        v->size--;
    }

    free(v->data);
    v = NULL;
}

#endif
