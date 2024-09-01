#ifndef dictionaries_c
#define dictionaries_c

#include "dictionaries.h"
#include <string.h>

void _dictionaries_print_frequency(string_virtual *frequency) {
	errors_panic("_dictionaries_print_frequency (frequency)", string_virtuals_check(frequency));

	ushort freq = 0;
	if (frequency->size > 1) {
		freq = frequency->text[0] - '0';

		if (freq > max_freq) freq = 0; 

		for (size_t i = 0; i < freq; i++) {
			printf("🬹🬹");
		}
	}

	for (size_t i = freq; i < max_freq; i++) {
		printf("🮏🮏");
	}
}

void dictionaries_print_note(const string *note) {
	//TODO: convert columns to tokens_next
	vector columns = vectors_init(vectors_string_virtual_type);
	strings_make_trim_virtual(note, &colsep, &columns);
	string_virtual *cols = (string_virtual *)columns.data;

	printf("\033[2mNotas \n\n");

	if (columns.size > 1) {
		printf("🮌 ");
		strings_print_no_panic((string *)(cols+1));
	}

	if (columns.size > 5) {
		printf(" - ");
		strings_print_no_panic((string *)(cols+5));
	}

	if (pages_columns_check(2)) {
		printf(" - [");
		strings_print_no_panic((string *)(cols+2));
		printf("] \n");
	} 

	printf("🮌 \n🮌 ");
	if (columns.size > 4) {
		strings_print_no_panic((string *)(cols+4));
		printf("\n");
	}

	printf("\n\n\033[0m");

	vectors_free(&columns);
}

void dictionaries_print(
	const string *line, size_t current, size_t total, 
	dictionaries_status dict_stat, dictionaries_sorting dict_sort
) {
	errors_panic("dictionaries_print (line)", strings_check_extra(line));
	errors_panic("dictionaries_print (dict_stat)", dict_stat < 0);


	screens_clear();
	printf("\033[31mLinTerm | Dicionário \033[0m| Item: %ld/%ld ", current + 1, total);

	switch (dict_sort) {
		case dictionaries_not_sort:
		break;
		case dictionaries_word_sort:
			printf("| ordem: alfabética ");
		break;
		case dictionaries_frequency_sort:
			printf("| ordem: frequência ");
		break;
	}

	switch (dict_stat) {
	case dictionaries_note_not_added_status: 
		printf("| \033[31mNota não adicionada\033[0m");
	break;
	case dictionaries_note_added_status: 
		printf("| \033[32mNota adicionada\033[0m");
	break;
	case dictionaries_note_exists_status: 
		printf("| \033[33mNota já existe\033[0m");
	break;
	case dictionaries_note_inserted_status: 
		printf("| \033[32mNota inserida\033[0m");
	break;
	case dictionaries_note_not_inserted_status: 
		printf("| \033[31mNota não inserida\033[0m");
	break;
	case dictionaries_note_removed_status:
		printf("| \033[32mNota removida\033[0m");
	break;
	case dictionaries_note_not_removed_status:
		printf("| \033[31mNota não removida\033[0m");
	break;
	case dictionaries_note_not_defined_status:
		printf("| \033[31mDefinição não adicionada\033[0m");
	break;
	case dictionaries_note_defined_status:
		printf("| \033[32mDefinição adicionada\033[0m");
	break;
	case dictionaries_note_categorized_status: 
		printf("| \033[32mNota adicionada\033[0m");
	break;
	case dictionaries_note_not_categorized_status: 
		printf("| \033[32mNota adicionada\033[0m");
	break;
	case dictionaries_filter_enabled_status:
		printf("| \033[32mFiltro ativado\033[0m");
	break;
	case dictionaries_filter_disabled_status:
		printf("| \033[32mFiltro desativado\033[0m");
	break;
	case dictionaries_filter_not_matched_status:
		printf("| \033[31mNenhuma correspondência\033[0m");
	break;
	}

	printf("\n\n\n");


	string_virtual tokens[dictionaries_columns_size];
	size_t tokens_size = strings_get_tokens(line, tokens, dictionaries_columns_size, &colsep);
	#define column_check(index) tokens_size > index && tokens[index].size > 1


	printf("\033[1m");
	if (column_check(2)) { 
		strings_print((string *)&tokens[2]);
		printf(" ");
	}

	if (column_check(0)) { 
		strings_print((string *)&tokens[0]);
	}

	if (column_check(3)) { 
		printf(" [");
		strings_print((string *)&tokens[3]);
		printf("] ");
	}  
	printf("\033[0m\n");

	if (column_check(4)) { 
		strings_print((string *)&tokens[4]);
		printf("\n");
	}

	if (column_check(5)) { 
		printf("\nExemplo: \n");
		strings_print((string *)&tokens[5]);
		printf("\n");
	}

	printf("\n\nFrequência: \n");
	_dictionaries_print_frequency(&tokens[1]);
	printf("\n\n\n\n");
}

bool dictionaries_compare_frequency(string *first, string *second) {
	errors_panic("dictionaries_compare_frequency (first)", strings_check_extra(first));
	errors_panic("dictionaries_compare_frequency (second)", strings_check_extra(second));

	char *first_next_sep = strchr(first->text, '=');
	ushort first_freq = 0;

	if (first_next_sep[1] > 0 && first_next_sep[1] - '0' <= max_freq) {
		first_freq = first_next_sep[1] - '0';
	}

	char *second_next_sep = strchr(second->text, '=');
	ushort second_freq = 0;

	if (second_next_sep[1] > 0 && second_next_sep[1] - '0' <= max_freq) {
		second_freq = second_next_sep[1] - '0';
	}

	if (first_freq > second_freq) {
		return false;
	} 

	return true;
}


#endif
