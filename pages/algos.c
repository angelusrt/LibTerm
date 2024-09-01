#ifndef algos_c
#define algos_c

#include "algos.h"

__attribute_warn_unused_result__
bool _doubles_compare(double *a, double *b) {
	return *a > *b;
}

vector algos_parameterize(const vector *lines, const char *filename) {
	set tokens_unique = files_make_trim_unique(filename, O_RDONLY, &special_characters);
	sets_sort_hashs(&tokens_unique);

	vector tokens_unique_type_hash = vectors_init(vectors_size_type);
	for (size_t i = 0; i < tokens_unique.hashs.size; i++) {
		vectors_push(&tokens_unique_type_hash, (void *)0);
	}

	vector parameters = vectors_init(vectors_vector_type);
	set freqs_unique = sets_init(vectors_size_type);
	set genders_unique = sets_init(vectors_string_virtual_type);
	set types_unique = sets_init(vectors_string_virtual_type);

	//

	const string *lines_data = lines->data;
	for (size_t i = 0; i < lines->size; i++) {
		const string *line = &lines_data[i];


		string_virtual tokens[dictionaries_columns_size - 1];
		size_t tokens_size = strings_get_tokens(line, tokens, dictionaries_columns_size - 1, &colsep);

		if(tokens_size < dictionaries_columns_size - 1) continue;

		string_virtual *mean_example = &tokens[dictionaries_columns_size - 2];
		mean_example->size = (line->text + line->size) - mean_example->text;


		size_t freq_num = tokens[1].text[0] - '0';
		if (freq_num <= 3) sets_push(&freqs_unique, (void *)freq_num);
		if (tokens[2].size > 1) sets_push(&genders_unique, &tokens[2]);
		if (tokens[3].size > 1) sets_push(&types_unique, &tokens[3]);

		size_t word_hash = strings_hasherize((string *)&tokens[0]);
		long pos = sets_bfind_hash(&tokens_unique, word_hash);
		if (pos < 0) continue;

		size_t type_hash = strings_hasherize((string *)&tokens[3]);
		((size_t *)tokens_unique_type_hash.data)[pos] = type_hash;
	}

	sets_sort_hashs(&freqs_unique);
	sets_sort_hashs(&genders_unique);
	sets_sort_hashs(&types_unique);

	//
	
	for (size_t i = 0; i < lines->size; i++) {
		string line = ((string *)lines->data)[i];

		string_virtual tokens[dictionaries_columns_size];
		size_t tokens_size = strings_get_tokens(&line, tokens, dictionaries_columns_size, &colsep);
		errors_panic("algos_parameterize (tokens_size mismatch)", tokens_size < dictionaries_columns_size);

		//TODO: solve -1
		size_t word_hash = strings_hasherize((string *)&tokens[0]);
		long word_hash_pos = sets_bfind_hash(&tokens_unique, word_hash);
		if (word_hash_pos < 0) word_hash_pos = 0;

		size_t word_hash_freq = ((size_t *)tokens_unique.freqs.data)[word_hash_pos];

		//

		size_t freq = tokens[1].text[0] - '0';
		size_t freq_freq = 0;

		if (freq < 4) {
			long freqs_pos = sets_bfind_hash(&freqs_unique, freq);

			if (freqs_pos >= 0) freq_freq = ((size_t *)freqs_unique.freqs.data)[freqs_pos];
		} else {
			freq = 0;
		}

		//
		
		size_t gender_hash = 0;
		long gender_hash_pos = 0;
		size_t gender_hash_freq = 0;

		if (tokens[2].size > 1) {
			gender_hash = strings_hasherize((string *)&tokens[2]);
			gender_hash_pos = sets_bfind_hash(&genders_unique, gender_hash);
		}

		if (gender_hash_pos >= 0) {
			gender_hash_freq = ((size_t *)genders_unique.freqs.data)[gender_hash_pos];
		}

		//
		
		size_t type_hash = 0;
		long type_hash_pos = 0;
		size_t type_hash_freq = 0;

		if (tokens[3].size > 1) {
			type_hash = strings_hasherize((string *)&tokens[3]);
			type_hash_pos = sets_bfind_hash(&types_unique, type_hash);
		}

		if (type_hash_pos >= 0) {
			type_hash_freq = ((size_t *)types_unique.freqs.data)[type_hash_pos];
		}

		//

		//TODO?: maybe convert to static
		vector words_freqs = vectors_init(vectors_size_type);
		vector words_sfreqs = vectors_init(vectors_size_type);

		string words_buffer;
		strings_preinit(words_buffer, strings_petit);

		size_t lesser_size = utils_min(words_buffer.size, tokens[4].size);
		strncpy(words_buffer.text, tokens[4].text, lesser_size);
		words_buffer.size = lesser_size;

		lesser_size = utils_min(words_buffer.capacity - lesser_size, tokens[5].size);
		strncat(words_buffer.text, tokens[5].text, lesser_size);
		words_buffer.size += lesser_size;
		words_buffer.text[words_buffer.size - 1] = '\0';

		strings_replace(&words_buffer, &special_characters, '\0');

		double word_size_mean = 0;
		string_virtual words_word = {.text = NULL, .size = 0};
		while (!strings_next_token(&words_buffer, &words_word, '\0')) {
			if (words_word.text[0] == '\0') continue;

			size_t words_word_hash = strings_hasherize((string *)&words_word);

			long hash_pos = sets_bfind_hash(&tokens_unique, words_word_hash);
			if (hash_pos < 0) continue;

			word_size_mean += words_word.size; 

			size_t words_word_freq = ((size_t *)tokens_unique.freqs.data)[hash_pos];
			size_t words_word_type_hash = ((size_t *)tokens_unique_type_hash.data)[hash_pos];
			vectors_push(&words_freqs, (void *)words_word_freq);

			if (words_word_type_hash != substantive_hash) continue;
			vectors_push(&words_sfreqs, (void *)words_word_freq);
		}

		vector_stats words_freqs_stat = vectors_stat(&words_freqs);
		vector_stats words_sfreqs_stat = vectors_stat(&words_sfreqs);

		//

		vector parameter = vectors_init(vectors_double_type);

		double word_parameters[algos_parameters_size] = {
			word_hash, 
			tokens[0].size,
			word_size_mean/words_freqs.size,

			word_hash_freq,
			freq_freq,
			gender_hash_freq,
			type_hash_freq,

			words_freqs.size,
			words_freqs_stat.min,
			words_freqs_stat.mean,
			words_freqs_stat.max,

			words_sfreqs.size,
			words_sfreqs_stat.min,
			words_sfreqs_stat.mean,
			words_sfreqs_stat.max,
		};

		for (size_t j = 0; j < algos_parameters_size; j++) {
			vectors_push(&parameter, &word_parameters[j]);
		}

		vectors_push(&parameters, &parameter);

		//

		vectors_free(&words_freqs);
		vectors_free(&words_sfreqs);
	}

	//
	
	sets_free(&freqs_unique);
	sets_free(&genders_unique);
	sets_free(&types_unique);
	vectors_free(&tokens_unique_type_hash);
	sets_free(&tokens_unique);

	vectors_normalize(&parameters);
	return parameters;
}

algo algos_make(const vector *note_lines, const vector *parameters) {
	errors_panic("algos_make (note_lines)", vectors_check(note_lines));
	errors_panic("algos_make (parameters)", vectors_check(parameters));

	const string *lines = note_lines->data;
	const vector *parameters_data = parameters->data;

	vector categories = vectors_init(vectors_string_type);
	vector categories_hash = vectors_init(vectors_size_type);
	vector categories_indexes = vectors_init(vectors_vector_type);

	for (size_t i = 0; i < note_lines->size; i++) {
		const string line = lines[i];

		string_virtual word = {.text=line.text, .size=pages_word_len};
		string_virtual category = {.text=line.text+pages_category_offset(0)+1, .size=pages_category_len};

		if (category.text[0] == '\0') continue;

		word.size = strnlen(word.text, pages_word_len) + 1;
		category.size = strnlen(category.text, pages_category_len) + 1;

		const size_t word_hash = strings_hasherize((string *)&word);
		const size_t category_hash = strings_hasherize((string *)&category);

		long parameter_index = -1;

		//TODO: binary search
		for (size_t j = 0; j < parameters->size; j++) {
			const vector parameter = parameters_data[j];
			const size_t hash = ((double *)parameter.data)[0];

			if (hash == word_hash) {
				parameter_index = j;
				break;
			}
		}

		if (parameter_index < 0) continue;

		long category_index = -1;
		for (size_t j = 0; j < categories_hash.size; j++) {
			size_t *categories_hash_data = categories_hash.data;
			if (categories_hash_data[j] == category_hash) {
				category_index = j;
				break;
			}
		}

		if (category_index < 0) {
			string category_buffer = strings_init(strings_min + 1);
			strncpy(category_buffer.text, category.text, category.size);
			category_buffer.size = category.size;

			vectors_push(&categories, &category_buffer);
			vectors_push(&categories_hash, (void *)category_hash);

			vector indexes = vectors_init(vectors_size_type);
			vectors_push(&indexes, (void *)parameter_index);
			vectors_push(&categories_indexes, &indexes);
		} else {
			vectors_push(&((vector *)categories_indexes.data)[category_index], (void *)parameter_index);
		}
	}

	vector categories_value_and_weights = vectors_init(vectors_vector_type);
	for (size_t j = 0; j < categories_indexes.size; j++) {
		const vector indexes = ((vector *)categories_indexes.data)[j];
		vector value_and_weights = algos_reduce(&indexes, parameters);

		vectors_push(&categories_value_and_weights, &value_and_weights);
	}

	vectors_free(&categories_indexes);

	return (algo) {
		.category=categories,
		.category_hash=categories_hash,
		.value_and_weights=categories_value_and_weights,
	};
}

bool algos_check(const algo *a) {
	if (errors_check("algos_check (a.category)", vectors_check(&a->category))) return true;
	if (errors_check("algos_check (a.category_hash)", vectors_check(&a->category_hash))) return true;
	if (errors_check("algos_check (a.value_and_weights)", vectors_check(&a->value_and_weights))) return true;

	return false;
} 

void algos_compare(const algo *a, const vector *parameter, vector *resemblances) {
	errors_panic("algos_compare (a)", algos_check(a));
	errors_panic("algos_compare (parameter)", vectors_check(parameter));
	errors_panic("algos_compare (resemblances)", vectors_check(resemblances));

	if (resemblances->size < a->category.size) {
		for (size_t i = resemblances->size; i < a->category.size; i++) {
			double zero = 0;
			vectors_push(resemblances, &zero);
		}
	} 

	for (size_t i = 0; i < a->value_and_weights.size; i++) {
		const vector value_and_weight = ((vector *)a->value_and_weights.data)[i];
		errors_panic("algos_compare (value_and_weight.size != 2)", value_and_weight.size != 2);

		const vector value = ((vector *)value_and_weight.data)[0];
		const double *value_data = value.data;

		vector weight = ((vector *)value_and_weight.data)[1];
		const double *weight_data = weight.data;

		double *parameter_data = parameter->data;

		bool is_valid = (value.size ^ weight.size) + (weight.size ^ parameter->size);
		errors_panic("algos_compare (value.size != weight.size != parameter.size)", is_valid);

		double resemblance = 0;
		double weight_sum = 0;
		for (size_t j = 1; j < value.size; j++) {
			double w = weight_data[j];
			double x = 0;

			double param = parameter_data[j];
			double val = value_data[j];

			errors_panic("algos_compare (param < 0)", param < 0);
			errors_panic("algos_compare (val < 0)", val < 0);

			if (param > val) {
				x = param - val;
			} else {
				x = val - param;
			}

			weight_sum += w;
			double c = utils_min((x * 2 * pow(20, w)), 1);
			resemblance += (w * (1 - c));
		}

		if (weight_sum != 0) {
			resemblance /= weight_sum;
		}

		((double *)resemblances->data)[i] = resemblance;
	}
}

void algos_free(algo *a) {
	vectors_free(&a->category);
	vectors_free(&a->category_hash);
	vectors_free(&a->value_and_weights);
}

vector algos_reduce(const vector *indexes, const vector *parameters) {
	errors_panic("algos_reduce (indexes)", vectors_check(indexes));
	errors_panic("algos_reduce (parameters)", vectors_check(parameters));
	errors_panic("algos_reduce (indexes.size == 0)", indexes->size == 0);
	errors_panic("algos_reduce (parameters.size == 0)", parameters->size == 0);

	const size_t *indexes_data = indexes->data;
	const vector *parameters_data = parameters->data;

	vector min = vectors_init(vectors_double_type);
	vector median = vectors_init(vectors_double_type);
	vector max = vectors_init(vectors_double_type);

	double zero = 0;
	double one = 1;

	const vector parameter = parameters_data[0];
	for (size_t i = 0; i < parameter.size; i++) {
		vectors_push(&min, &one);
		vectors_push(&median, &zero);
		vectors_push(&max, &zero);
	}

	double *min_data = min.data;
	double *median_data = median.data;
	double *max_data = max.data;

	for (size_t i = 0; i < indexes->size; i++) {
		const size_t index = indexes_data[i];
		const vector parameter = parameters_data[index];

		//Dont consider hash index
		for (size_t j = 1; j < parameter.size; j++) {
			double param = ((double *)parameter.data)[j];

			if (param < min_data[j]) {
				min_data[j] = param;
			}

			if (param > max_data[i]) {
				max_data[j] = param;
			}
		}
	}

	for (size_t i = 1; i < parameter.size; i++) {
		vector meantemp = vectors_init(vectors_double_type);

		for (size_t j = 0; j < indexes->size; j++) {
			const size_t index = indexes_data[j];
			const vector parameter = parameters_data[index];

			double param = ((double *)parameter.data)[i];
			vectors_push(&meantemp, &param);
		}

		if (meantemp.size == 0) {
			vectors_free(&meantemp);
			continue;
		}
		vectors_sort(&meantemp, (compfunc)_doubles_compare);

		double *m = meantemp.data;
		if (meantemp.size % 2 == 0) {
			median_data[i] = (m[meantemp.size/2 - 1] + m[meantemp.size/2])/2;
		} else {
			median_data[i] = m[(meantemp.size - 1)/2];
		}

		vectors_free(&meantemp);
	}

	//divide mean by total to get real mean
	for (size_t i = 1; i < max.size; i++) {
		//max now means weight
		max_data[i] = 1 - (max_data[i] - min_data[i]);
	}

	vectors_free(&min);

	vector value_and_weight = vectors_init(vectors_vector_type);
	vectors_push(&value_and_weight, &median);
	vectors_push(&value_and_weight, &max);

	return value_and_weight;
}

void _algos_print(const vector *parameter) {
	const double *params = ((double *)parameter->data);

	//printf("🮌 hash: ");
	//printf("%zu ", (size_t)params[0]);
	//printf("\n🮌\n");
	utils_print_float("🮌 size: ", params[1]);
	utils_print_float("🮌 letters: ", params[2]);
	printf("\n🮌\n");
	utils_print_float("🮌 freq: ", params[3]);
	utils_print_float("🮌 freqs: ", params[4]);
	utils_print_float("🮌 gen: ", params[5]);
	utils_print_float("🮌 tipo: ", params[6]);
	printf("\n🮌\n");
	utils_print_float("🮌 quant: ", params[7]);
	utils_print_float("🮌 min: ", params[8]);
	utils_print_float("🮌 med: ", params[9]);
	utils_print_float("🮌 max: ", params[10]);
	printf("\n🮌\n");
	utils_print_float("🮌 squant: ", params[11]);
	utils_print_float("🮌 smin: ", params[12]);
	utils_print_float("🮌 smed: ", params[13]);
	utils_print_float("🮌 smax: ", params[14]);
	printf("\033[0m \n\n");
}
void algos_print(const vector *parameter) {
	errors_panic("algos_print (parameter)", vectors_check(parameter));
	errors_panic("algos_print (parameter.size)", parameter->size != algos_parameters_size);
	_algos_print(parameter);
}

void algos_print2(const algo *a, size_t cursor) {
	errors_panic("algos_print (a)", algos_check(a));

	screens_clear();
	printf("\033[31mLinTerm | Categorias \033[0m| Item: %ld/%ld \n\n\n", cursor + 1, a->category.size);

	const vector value_and_weights = ((vector *)a->value_and_weights.data)[cursor];
	const vector value = ((vector *)value_and_weights.data)[0];
	const vector weight = ((vector *)value_and_weights.data)[1];

	strings_print(&((string *)a->category.data)[cursor]);
	printf("\n\n\n\n\033[2mAlgos \n\n");
	_algos_print(&value);

	printf("\033[2mPesos \n\n");
	_algos_print(&weight);

}

void algos_predict(const vector *categories, const vector *resemblance) {
	errors_panic("algos_predict (categories)", vectors_check(categories));
	errors_panic("algos_predict (resemblance)", vectors_check(resemblance));

	//errors_debug("algos_predict(#categories: %zu, #resemblance: %zu)", categories->size, resemblance->size);

	bool has_equal_size = categories->size != resemblance->size;
	errors_panic("algos_predict (#categories != #resemblance)", has_equal_size);

	const string *categories_data = categories->data;
	const double *resemblance_data = resemblance->data;

	printf("\033[2mPrevisão \n\n");

	for (size_t i = 0; i < categories->size; i++) {
		printf("🮌 ");
		strings_print(&categories_data[i]);
		printf(": ");
		printf("%.4f ", resemblance_data[i]);
	}

	printf("\033[0m \n\n");

}

#endif
