#ifndef strings_c
#define strings_c
#pragma once

#include "strings.h"
#include "vectors.c"

string strings_init(const strings_size size) {
	char *text = calloc(size, sizeof(char));
    errors_panic("strings_init (text)", text == NULL);

    return (string) {.size=1, .capacity=size, .text=text};
}

string strings_make_copy(const string *s) {
	errors_panic("strings_copy (s)", string_virtuals_check((string_virtual *)s));
		
	string dest = strings_init(s->size);

	for (size_t j = 0; j < s->size; j++) {
		dest.text[j] = s->text[j];
	}

	dest.size = s->size;

	return dest;
}

string strings_make(const char *lit) {
    errors_panic("strings_make (lit)", lit == NULL);
    errors_warn("strings_make lit == '\\0'", strncmp(lit, "\0", 1) == 0);

	size_t new_size = strlen(lit) + 1;
	size_t new_capacity = strings_min;
	while (new_capacity < new_size) {
		new_capacity = new_capacity << 1;
	}

	string new_string = strings_init(new_capacity);
    new_string.size = new_size;

    strncpy(new_string.text, lit, new_string.capacity);
    new_string.text[new_string.size - 1] = '\0';
    new_string.text[new_string.capacity - 1] = '\0';

    return new_string;
}

bool strings_upgrade(string *s) {
	errors_panic("strings_upgrade (s)", s);
	errors_panic("strings_upgrade (s.size == 0)", s->size == 0);
	errors_panic("strings_upgrade (s.capacity < strings_min)", s->capacity < strings_min);

	bool less_than_half_capacity = s->size - 1 < (s->capacity >> 1);
	bool bigger_than_min = s->size - 1 > strings_min;
	bool almost_big_as_capacity = s->size >= s->capacity - 1;
	bool almost_big_as_max = s->capacity >= (SIZE_MAX >> 1);

	if (almost_big_as_max) {
		return true;
	}

	size_t new_capacity = 0;
	if (almost_big_as_capacity) {
		new_capacity = s->capacity << 1;
	} else if (less_than_half_capacity && bigger_than_min) {
		new_capacity = s->capacity >> 1;
	}

	if (almost_big_as_capacity || (less_than_half_capacity && bigger_than_min)) {
		char *new_text = realloc(s->text, new_capacity*sizeof(char));
		errors_warn("realloc falhou - prosseguindo sem.", new_text == NULL);

		if (new_text == NULL) { return true; }

		s->text = new_text;
		s->capacity = new_capacity;
		s->text[s->capacity - 1] = '\0';
	}

	return false;
}

void strings_free(string *s) {
	if (s == NULL) { 
		printf(colors_warn("strings_free (s == NULL)"));
		return; 
	}

    free(s->text);
    s->text = NULL;
    s = NULL;
}

void strings_free_many(size_t n, string *s, ...) {
    errors_panic("strings_free (n == 0)", n == 0);
    errors_panic("strings_free (s == 0)", s == 0);

    va_list args;
    va_start(args, s);
	strings_free(s);

    for (size_t i = 0; i < (size_t)(n - 1); i++) {
        string *arg_s = va_arg(args, string *);

		if (arg_s == NULL) {
			printf(colors_warn("strings_free (arg_s == NULL)"));
			continue;
		}

		strings_free(arg_s);
    }

    va_end(args);
}

bool strings_compare(const string *first, const string *second) {
    errors_panic("strings_compare (first)", strings_check_extra(first));
    errors_panic("strings_compare (sec)", strings_check_extra(second));

    size_t size = first->size;
    if (first->size > second->size) {
        size = second->size;
    }

    return strncmp(first->text, second->text, size) > 0;
}

//checks if string was properly initialized
bool strings_check(const string *s) {
	if (errors_check("strings_check (s)", s == NULL)) return true;
	if (errors_check("strings_check (s.text)", s->text == NULL)) return true;
	if (errors_check("strings_check (s.size == 0)", s->size == 0)) return true;
	//if (errors_check("strings_check (s.capacity < strings_min)", s->capacity < strings_min)) return true;
	//TODO: uncomment previous statement

	return false;
}

//checks if string was properly assigned
bool strings_check_extra(const string *s) {
	if (strings_check(s)) return true;
	if (errors_check("strings_check_extra (s.text[0] == '\\0')", s->text[0] == '\0')) return true;
	if (errors_check("strings_check_extra (s.size == 1)", s->size == 1)) return true;

	return false;
}

bool string_virtuals_check(const string_virtual *s) {
	if (errors_check("string_virtuals_check (s)", s == NULL)) return true;
	if (errors_check("string_virtuals_check (s.text)", s->text == NULL)) return true;
	if (errors_check("string_virtuals_check (s.size <= 1)", s->size <= 1)) return true;

	return false;
}

vector strings_make_find(const string *s, const string *sep) {
	errors_panic("strings_make_find (s)", strings_check_extra(s));
	errors_panic("strings_make_find (sep)", strings_check_extra(sep));

	vector indexes = vectors_init(sizeof(size_t));

    for (size_t i = 0, j = 0; i < s->size - 1; i++) {
        if (s->text[i] == sep->text[0]) {
            j = 1;
		} else if (s->text[i] == sep->text[j]) {
            j++;
		} else {
            j = 0;
        }

        if (j == sep->size - 1) {
			vectors_push(&indexes, (void *)(i - (j - 1)));
        }
    }

    return indexes;
}

void strings_replace(string *s, const string *seps, const char rep) {
    errors_panic("strings_replace (s)", strings_check_extra(s));
    errors_panic("strings_replace (seps)", strings_check_extra(seps));

    for (size_t i = 0; i < s->size - 1; i++) {
		for (size_t j = 0; j < seps->size - 1; j++) {
			if (s->text[i] == seps->text[j]) {
				s->text[i] = rep;
				break;
			} 
		}
    }
}

string strings_make_replace(const string *s, const string *sep, const string *rep) {
    errors_panic("strings_make_replace (s)", s == NULL);
    errors_panic("strings_make_replace (sep)", sep == NULL);
    errors_panic("strings_make_replace (rep)", rep == NULL);
    errors_panic("strings_make_replace (s.size <= 1)", s->size <= 1);
    errors_panic("strings_make_replace (sep.size <= 1)", sep->size <= 1);
    errors_panic("strings_make_replace (rep.size == 0)", rep->size == 0);

    long diff_size = (rep->size - sep->size);
    size_t size = s->size + (sep->size - 1) * diff_size;

	vector indexes = strings_make_find(s, sep);
	if (indexes.size == 0) {
		vectors_free(&indexes);
		return strings_init(strings_min);
	}

	size_t capacity = strings_min;
	while (capacity < size) {
		capacity = capacity << 1;
	}

	string new_string = strings_init(capacity);
    new_string.size = size;

    size_t prev = 0, i = 0, j = 0;
    while (i < s->size - 1) {
		size_t index = ((size_t *)indexes.data)[j];
		size_t offset = i + j * diff_size;
		size_t sentence_size = 0;

        if (j < indexes.size && index == i) {
            strncpy(new_string.text + offset, rep->text, rep->size);
            prev = index + sep->size - 1;
            i += sep->size - 1;
            j += 1;
        } else {
            if (j > indexes.size - 1) {
                sentence_size = s->size - prev;
            } else {
                sentence_size = index - prev;
            }

            strncpy(new_string.text + offset, s->text + prev, sentence_size);
            i += sentence_size;
        }
    }

	vectors_free(&indexes);
    return new_string;
}

bool strings_next_token(const string *s, string_virtual *prev, char token) {
	errors_panic("strings_next_token (s)", strings_check(s));

	bool has_text_been_setted = false;
	if (prev->text == NULL) {
		prev->text = s->text;
		prev->size = 2;
		has_text_been_setted = true;
	}
	
	//errors_panic("strings_next_token (prev)", string_virtuals_check(prev));
	errors_panic("strings_next_token (prev < s)", prev->text < s->text);
	errors_panic("strings_next_token (prev > s.size)", prev->text > s->text + s->size);

	long new_size = 0;
	do {
		size_t token_pos = prev->text - s->text;
		if (token_pos > s->size) return true;
		//errors_panic("strings_next_token (token_pos > s.size)", token_pos > s->size);

		size_t distance_from_end = s->size - token_pos;
		if (distance_from_end == 0) return true;

		if (has_text_been_setted) {
			char *next_separator = strchr(prev->text, token);
			if (next_separator == NULL) {
				next_separator = s->text + s->size;
			}

			new_size = (next_separator + 1) - prev->text;
			if (new_size <= 0) return true;

			prev->size = new_size;

			has_text_been_setted = next_separator == NULL ? true : false;
			break;

			//if (prev->text[0] != token) break;
		} else {
			prev->text += prev->size;
			has_text_been_setted = true;
		}
	} while (true);

	return false;
}

size_t strings_get_tokens(const string *line, string_virtual *tokens, size_t tokens_len, const string *separator) {
	string_virtual token = {.text=NULL, .size=0};

	size_t index = 0;
	while (!strings_next_token(line, &token, separator->text[0])) {
		tokens[index] = token;
		index++;
		if (index >= tokens_len) break;
	}

	return index;
}

vector strings_trim(const string *s, const string *sep) {
	errors_panic("strings_trim (s)", strings_check_extra(s));
	errors_panic("strings_trim (sep)", strings_check_extra(sep));

	vector indexes = strings_make_find(s, sep);
    vector sentences = vectors_init(vectors_string_type);

	if (indexes.size == 0) {
		vectors_free(&indexes);
		printf(colors_warn("strings_trim (indexes.size == 0)"));
		return sentences;
	}

    size_t prev = 0, size = 0;
    for (size_t i = 0; i < indexes.size + 1; i++) {
		size_t index = ((size_t *)indexes.data)[i];

        if (i > indexes.size - 1) {
            size = s->size - prev + 1;
        } else {
            size = index - prev + 1;
        }

        if (size <= 1) {
            prev = index + (sep->size - 1);
            continue;
        }

        if (s->text[prev] == '\0') {
            continue;
        }

		size_t capacity = strings_min;
		while (capacity < size) {
			capacity = capacity << 1;
		}

		string new_string = strings_init(capacity);
        new_string.size = size;

        //strncpy(new_string.text, s->text + prev, size - 1);
		for (size_t i = 0; i < size; i++) {
			new_string.text[i] = (s->text + prev)[i];
		}

        new_string.text[size - 1] = '\0';

        vectors_push(&sentences, &new_string);

        if (i > indexes.size - 1) {
            break;
        }

        //" Ä" -> {194, 160, 195, 132}
        // if (i == 34) { printf("%zu %zu %zu\n", prev, sf->indexes[i],
        // sf->indexes[i+1]); }

        prev = index + (sep->size - 1);
    }

	vectors_free(&indexes);
    return sentences;
}

void strings_make_trim_virtual(const string *s, const string *sep, vector *sentences) {
    errors_panic("strings_make_trim_virtual (s)", s == NULL);
    errors_panic("strings_make_trim_virtual (sep)", sep == NULL);
    errors_panic("strings_make_trim_virtual (s.size <= 1)", s->size <= 1);
    errors_panic("strings_make_trim_virtual (sep.size <= 1)", sep->size <= 1);

	vector indexes = strings_make_find(s, sep);
	if (indexes.size == 0) {
		//printf(colors_warn("strings_trim_virtual (indexes.size == 0)"));
		sentences->size = 0;
		vectors_free(&indexes);
		return;
	}

	for (size_t i = 0; i < sentences->size; i++) {
		string_virtual *sv = (string_virtual *)sentences->data+i;
		sv->size = 0;
	}

	string_virtual new_string_virtual;
    size_t prev = 0, size = 0;
    for (size_t i = 0; i < indexes.size + 1; i++) {
		size_t index = ((size_t *)indexes.data)[i];

        if (i >= indexes.size) {
            size = s->size - prev + 1;
        } else {
            size = index - prev + 1;
        }

		//if (s->text[prev] == '\0') { continue; }

        if (size <= 1) {
            prev = index + (sep->size - 1);
        }

		if (sentences->size <= i) {
			new_string_virtual.size = size; 
			new_string_virtual.text = s->text + prev;
			vectors_push(sentences, &new_string_virtual);
		} else {
			string_virtual *old_string_virtual = (string_virtual *)sentences->data+i;
			old_string_virtual->size = size;
			old_string_virtual->text = s->text + prev;
		}

        prev = index + (sep->size - 1);
    }

	vectors_free(&indexes);
}

set strings_make_trim_unique(const string *s) {
	errors_panic("strings_make_trim_unique (s)", strings_check(s));

	set tokens_unique = sets_init(vectors_string_type);

	string token = strings_make(s->text);
	bool push_stat = sets_push(&tokens_unique, &token);
	if (push_stat) {
		strings_free(&token);
	}

	for (size_t i = 0; i < s->size - 1; i++) {
		if (s->text[i] == '\0' && s->text[i + 1] != '\0') {
			token = strings_make(s->text+i+1);
			push_stat = sets_push(&tokens_unique, &token);
			if (push_stat) {
				strings_free(&token);
			}
		} 
	}
	
	return tokens_unique;
}

string strings_make_format(const char *const form, ...) {
    errors_panic("strings_make_format (form)", form == NULL);
    errors_panic("strings_make_format (form < 1)", strlen(form) < 1);

    va_list args, args2;
    va_start(args, form);
    *args2 = *args;

    size_t buff_size = vsnprintf(NULL, 0, form, args) + 1;

	size_t new_capacity = strings_min;
	while (new_capacity < buff_size) {
		new_capacity = new_capacity << 1;
	}

    char *text = calloc(new_capacity, sizeof(char));
    errors_panic("strings_make_format (text)", text == NULL);

    vsnprintf(text, buff_size, form, args2);

    va_end(args);

    return (string){.text=text, .size=buff_size, .capacity=new_capacity};
}

size_t strings_hasherize(const string *s) {
    errors_panic("strings_hasherize (s)", s == NULL);
    size_t hash = 0;

    for (size_t i = 0; i < s->size - 1; i++) {
        unsigned char character = s->text[i];
        size_t power = pow(hash_const, i + 1);
        hash += character * power;
    }

    return hash;
}

void strings_print(const string *s) {
    errors_panic("strings_print (s)", s == NULL);
    errors_panic("strings_print (s.size <= 1)", s->size <= 1);

    for (size_t i = 0; i < s->size - 1; i++) {
        printf("%c", s->text[i]);
    }
}

void strings_print_no_panic(const string *s) {
	if (s != NULL && s->size > 2) {
		strings_print(s);
	}
}

void strings_println(const string *s) {
	strings_print(s);
    printf("\n");
}

#endif
