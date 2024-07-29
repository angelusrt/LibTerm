#ifndef utils_c
#define utils_c

#include "utils.h"

__attribute_warn_unused_result__
int utils_find_line_aux(int f, string *buff, size_t hash) {
	bool file_read_ended = files_read(f, buff);
	if (file_read_ended) {
		return -1;
	}

	size_t len = strlen(buff->text);
	string_virtual sv = {.text=buff->text, .size=len+1};
	size_t sv_hash = strings_hasherize((string *)&sv);

	if (sv_hash == hash) {
		return 1;
	}

	return 0;
}

// If line was found, the index is returned else the code status -1
long utils_find_line(int file, string *word, size_t bytes) {
	size_t hash = strings_hasherize(word);
	string compare = strings_init(bytes);

	lseek(file, 0, SEEK_SET);

	long line_index = -1;
	int found_line = 0;
	while (found_line == 0) {
		found_line = utils_find_line_aux(file, &compare, hash);
		line_index++;
	}

	strings_free(&compare);

	return found_line == 1 ? line_index : -1;
}

void utils_print_frequency(string_virtual *frequency) {
	#define max_freq 5
	ushort freq = 0;

	if (frequency->size > 1) {
		freq = frequency->text[0] - '0';

		if (freq > 5) { 
			freq = 0; 
		}

		for (size_t i = 0; i < freq; i++) {
			printf("▣");
		}
	}

	for (size_t i = freq; i < max_freq; i++) {
		printf("□");
	}
}

#endif
