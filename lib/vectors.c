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
    case vectors_double_type:
        return sizeof(double);
    case vectors_float_type:
        return sizeof(float);
    }

    return sizeof(size_t);
}

size_t vectors_types_hasherize(vectors_type type, void *item) {
    switch (type) {
    case vectors_string_type:
        return strings_hasherize((string *)item);
    case vectors_size_type:
		return (size_t)item;
    case vectors_string_virtual_type:
        return strings_hasherize((string *)(string_virtual *)item);
    case vectors_double_type:
        return *(double *)item * 1000;
    case vectors_float_type:
        return *(float *)item * 1000;
    }

    return (size_t)item;
}


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
    case vectors_double_type:
		((double *)data)[data_index] = ((double *)item)[item_index];
        return;
	case vectors_float_type:
		((float *)data)[data_index] = ((float *)item)[item_index];
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
    case vectors_double_type:
		return (((double *)item) + item_index);
    case vectors_float_type:
		return (((float *)item) + item_index);
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
	case vectors_double_type:
	case vectors_float_type:
        return;
    }

    return;
}

vector vectors_init(vectors_type type) {
    size_t type_size = vectors_types_size(type);
    void *data = malloc(vectors_min * type_size);
    errors_panic("vectors_init (data)", data == NULL);

    return (vector){.size = 0, .capacity = vectors_min, .type = type, .data = data};
}

bool vectors_check(const vector *v) {
    if (errors_check("vectors_check (v)", v == NULL)) return true;
    if (errors_check("vectors_check (v.data)", v->data == NULL)) return true;
    if (errors_check("vectors_check (v.capacity < vectors_min)", v->capacity < vectors_min)) return true;

    return false;
}

void vectors_debug(vector *v, const char *const lit) {
	printf("%s<vector>{type: %d, size: %zu, capacity: %zu, data: %p}\n", lit, v->type, v->size, v->capacity, v->data);
}

bool vectors_push(vector *v, void *item) {
    errors_panic("vectors_push (v)", v == NULL);

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

vector_stats vectors_stat(vector *v) {
	double min = 0;
	double max = 0;
	double mean = 0;
	double total = 0;

	for (size_t j = 0; j < v->size; j++) {
		size_t freq = ((size_t *)v->data)[j];

		if (min == 0 || (min > 0 && freq < min)) {
			min = freq;
		} 

		if (max == 0 || (max > 0 && freq > max)) {
			max = freq;
		} 

		total += freq;
	}

	if (v->size > 0) {
		mean = (double)total/v->size;
	}

	return (vector_stats) {.min=min, .mean=mean, .max=max, .total=total};
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

void vectors_normalize(vector *v) {
	errors_panic("vectors_normalize (v)", vectors_check(v));
	errors_panic("vectors_normalize (v.type != vector)", v->type != vectors_vector_type);

	vector *lines = v->data;

	vector maxs = vectors_init(vectors_double_type);
	for (size_t j = 0; j < lines[0].size; j++) {
		double zero = 0;
		vectors_push(&maxs, &zero);
	}
	double *maxs_data = maxs.data;

	for (size_t i = 0; i < v->size; i++) {
		double *line = lines[i].data;

		errors_panic("vectors_normalize (lines[i])", vectors_check(&lines[i]));
		errors_panic("vectors_normalize (lines[i].type != double)", lines[i].type != vectors_double_type);

		for (size_t j = 1; j < lines[i].size; j++) {
			if (line[j] != INFINITY && line[j] > maxs_data[j]) {
				maxs_data[j] = line[j];
			}
		}
	}

	for (size_t i = 0; i < v->size; i++) {
		double *line = lines[i].data;

		for (size_t j = 1; j < lines[i].size; j++) {
			line[j] /= maxs_data[j]; 
		}
	}

	vectors_free(&maxs);
}


set sets_init(vectors_type type) {
	vector hashs = vectors_init(vectors_size_type); 
	vector freqs = vectors_init(vectors_size_type); 
	vector itens = vectors_init(type); 

	return (set){.hashs=hashs, .freqs=freqs, .itens=itens};
}

bool sets_check(const set *s) {
	if(errors_check("sets_check (s)", s == NULL)) return true;
	if(errors_check("sets_check (s.hashs)", vectors_check(&s->hashs))) return true;
	if(errors_check("sets_check (s.freqs)", vectors_check(&s->freqs))) return true;
	if(errors_check("sets_check (s.itens)", vectors_check(&s->itens))) return true;

	bool is_size_valid = ((s->hashs.size ^ s->freqs.size) + (s->freqs.size ^ s->itens.size)) == 0;
	if(errors_check("sets_check (s.size)", !is_size_valid)) return true;

	return false;
}

bool sets_push(set *s, void *item) {
	errors_panic("sets_push (s)", sets_check(s));

	size_t item_hash = vectors_types_hasherize(s->itens.type, item);
	for (size_t i = 0; i < s->hashs.size; i++) {
		size_t hash = ((size_t *)s->hashs.data)[i];

		if (item_hash == hash) {
			((size_t *)s->freqs.data)[i]++;
			return true;
		}
	}

	vectors_push(&s->hashs, (void *)item_hash);
	vectors_push(&s->freqs, (void *)1);
	vectors_push(&s->itens, item);

	return false;
}

void sets_sort(set *s, sets_compfunc is_current_bigger) {
	errors_panic("sets_sort (s)", sets_check(s));

	for (size_t i = s->itens.size; i > 0; i--) {
		for (size_t j = 1; j < i; j++) {
			if (!is_current_bigger(s, j)) continue;

			size_t *freq = (size_t *)s->freqs.data;
			size_t freq_prev = freq[j - 1];
			freq[j - 1] = freq[j];
			freq[j] = freq_prev;

			size_t *hash = (size_t *)s->hashs.data;
			size_t hash_prev = hash[j - 1];
			hash[j - 1] = hash[j];
			hash[j] = hash_prev;

			vectors_types_insert(s->itens.type, s->itens.data, s->itens.capacity - 1, s->itens.data, j); 				
			vectors_types_insert(s->itens.type, s->itens.data, j, s->itens.data, j - 1);
			vectors_types_insert(s->itens.type, s->itens.data, j - 1, s->itens.data, s->itens.capacity - 1);
		}
	}
}

bool _sets_compare_freqs(set *s, size_t index) {
	size_t *freq = (size_t *)s->freqs.data;
	size_t freq_prev = freq[index - 1];
	size_t freq_curr = freq[index];

	return freq_curr > freq_prev;
}

bool _sets_compare_hashs(set *s, size_t index) {
	size_t *hash = (size_t *)s->hashs.data;
	size_t hash_prev = hash[index - 1];
	size_t hash_curr = hash[index];

	return hash_curr > hash_prev;
}

void sets_sort_freqs(set *s) {
	errors_panic("sets_sort_freqs (s)", sets_check(s));
	sets_sort(s, _sets_compare_freqs);
}

void sets_sort_hashs(set *s) {
	errors_panic("sets_sort_hashs (s)", sets_check(s));
	sets_sort(s, _sets_compare_hashs);
}

size_t sets_freqs_biggest(const set *s) {
	errors_panic("sets_freqs_biggest (s)", sets_check(s));

	size_t max = 0;
	for(size_t i = 0; i < s->itens.size; i++) {
		size_t freq = ((size_t *)s->freqs.data)[i];
		if (freq > max) {
			max = freq;
		}
	}

	return max;
}

size_t sets_freqs_total(const set *s) {
	errors_panic("sets_sort_freqs (s)", sets_check(s));

	size_t freqs_total = 0;
	for(size_t i = 0; i < s->itens.size; i++) {
		size_t freq = ((size_t *)s->freqs.data)[i];

		freqs_total += freq;
	}

	return freqs_total;
}

long sets_find_hash(const set *s, size_t hash) {
	errors_panic("sets_find_hash (s)", sets_check(s));

	if (s->hashs.size == 0) return -1;
	size_t *hashs = (size_t *)s->hashs.data;

	for (size_t i = 0; i < s->hashs.size; i++) {
		if (hashs[i] == hash) return i;
	}

	return -1;
}

long sets_bfind_hash(const set *s, size_t hash) {
	errors_panic("sets_bfind_hash (s)", sets_check(s));

	if (s->hashs.size == 0) return -1;

	long lower_pos = 0;
	long higher_pos = (long)s->hashs.size - 1;

	while (lower_pos <= higher_pos) {
		long mid_pos = (lower_pos + higher_pos)/2;
		long shash = (long)((size_t *)s->hashs.data)[mid_pos];

		if (shash < (long)hash) {
			higher_pos = mid_pos - 1;
		} else if (shash > (long)hash) {
			lower_pos = mid_pos + 1;
		} else {
			return mid_pos;
		}
	} 

	return -1;
}

void sets_free(set *s) {
	if (s == NULL) return;

	vectors_free(&s->freqs);
	vectors_free(&s->hashs);
	vectors_free(&s->itens);

	s = NULL;
}

#endif
