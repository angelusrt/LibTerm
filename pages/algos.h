#ifndef algos_h
#define algos_h

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/cdefs.h>

#include "../lib/files.h"
#include "../lib/files.c"
#include "../lib/strings.h"
#include "../lib/strings.c"
#include "../lib/vectors.h"
#include "../lib/vectors.c"
#include "pages.h"
#include "pages.c"
#include "dictionaries.h"
#include "dictionaries.c"

#define utils_min(a, b) a > b ? b : a

#define utils_max(a, b) a > b ? a : b

#define utils_print_float(description, value) \
	printf(description); \
	printf("%.4f ", value)


const string substantive = strings_premake("Substantiv");
#define substantive_hash 38342192371

const string special_characters = strings_premake(";,.()[]= \n0123456789 ");

#define algos_parameters_size 15

typedef struct algo {
	vector category;
	vector category_hash;
	vector value_and_weights;
} algo;

__attribute_warn_unused_result__
vector algos_parameterize(const vector *lines, const char *filename);

void algos_print(const vector *parameter);

void algos_print2(const algo *a, size_t cursor);

__attribute_warn_unused_result__
bool algos_check(const algo *a);

__attribute_warn_unused_result__
vector algos_reduce(const vector *indexes, const vector *parameters);

__attribute_warn_unused_result__
algo algos_make(const vector *note_lines, const vector *parameters);

void algos_compare(const algo *a, const vector *parameter, vector *categories_resemblance);

void algos_predict(const vector *categories, const vector *resemblance);

void algos_free(algo *a);

#endif
