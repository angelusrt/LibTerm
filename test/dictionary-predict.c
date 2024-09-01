#include "../pages/algos.h"
#include "../pages/algos.c"

#define filename "../state/dictionary.csv"
#define filename2 "../state/apprentice.csv"

int main () {
    vector dict_lines = pages_make(filename, O_RDONLY);
	const string *dict = dict_lines.data;

    vector note_lines = pages_make(filename2, O_RDONLY | O_CREAT);

	vector dict_parameters = algos_parameterize(&dict_lines, filename);
	const vector *dict_data = dict_parameters.data;

	algo al = algos_make(&note_lines, &dict_parameters);
	vector resemblance = vectors_init(vectors_double_type);


	printf("word, ");
	for (size_t i = 0; i < al.category.size; i++) {
		string category = ((string *)al.category.data)[i];
		strings_print(&category);
		printf(", ");
	}

	printf("size, ");
	printf("letters, ");
	printf("freq, ");
	printf("freqs, ");
	printf("gen, ");
	printf("tipo, ");
	printf("quant, ");
	printf("min, ");
	printf("med, ");
	printf("max, ");
	printf("squant, ");
	printf("smin, ");
	printf("smed, ");
	printf("smax\n");

	for (size_t i = 0; i < dict_lines.size; i++) {
		string_virtual token = {.text=NULL, .size=0};
		if(strings_next_token(&dict[i], &token, '=')) continue;

		strings_print((string *)&token);
		printf(", ");

		algos_compare(&al, &dict_data[i], &resemblance); 
		for (size_t j = 0; j < resemblance.size; j++) {
			printf(" %f,", ((double *)resemblance.data)[j]);
		}

		const double *d = dict_data[i].data;
		for (size_t j = 1; j < dict_data[i].size; j++) {
			printf(" %f,", d[j]);
		}

		printf("\n");
	}

    vectors_free(&dict_lines);
    vectors_free(&note_lines);
	vectors_free(&dict_parameters);
	vectors_free(&resemblance);
	algos_free(&al);

	return 0;
}
