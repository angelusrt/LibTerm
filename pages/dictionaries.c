#ifndef dictionaries_c
#define dictionaries_c

#include "dictionaries.h"
void _dictionaries_print_frequency(string_virtual *frequency) {
	errors_panic("_dictionaries_print_frequency (frequency)", string_virtuals_check(frequency));

	#define max_freq 5
	ushort freq = 0;

	if (frequency->size > 1) {
		freq = frequency->text[0] - '0';

		if (freq > 5) freq = 0; 

		for (size_t i = 0; i < freq; i++) {
			printf("▣");
		}
	}

	for (size_t i = freq; i < max_freq; i++) {
		printf("□");
	}
}

void dictionaries_print(
	const string *line, size_t current, size_t total, const string_virtual *note, dictionaries_status dict_stat
) {
	errors_panic("dictionaries_print (line)", strings_check_extra(line));
	errors_panic("dictionaries_print (dict_stat)", dict_stat < 0);

	vector columns = vectors_init(vectors_string_virtual_type);
	strings_trim_virtual(line, &colsep, &columns);

	string_virtual *cols = (string_virtual *)columns.data;

	screens_clear();
	printf("\033[31mLinTerm | Dicionário \033[0m| Item: %ld/%ld ", current + 1, total);

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
	}

	printf("\n\n");

	_dictionaries_print_frequency(cols+1);
	printf("\n");

	printf(note->size > 1 ? "\033[32;1m" : "\033[1m");
	if (columns.size > 1) { 
		strings_print_no_panic((string *)(cols+0));
	}

	if (columns.size > 3 && cols[2].size > 1) { 
		printf(", ");
		strings_print((string *)(cols+2));
	}

	if (columns.size > 4) { 
		printf(" (");
		strings_print_no_panic((string *)(cols+3));
		printf(") ");
		printf("\033[0m\n");
	}  

	if (columns.size > 5) { 
		strings_print_no_panic((string *)(cols+4));
		printf("\n\n");
	}

	printf("Exemplo: \n");
	if (note->size > 1) {
		strings_print_no_panic((string *)note);
		printf("\n");
	}

	vectors_free(&columns);
}

#endif
