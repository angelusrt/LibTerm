#ifndef pages_c
#define pages_c

#include "pages.h"

void menus_print() {
	screens_clear();

	printf("\033[31mLinTerm | Menu\033[0m\n\n");
	printf("LinTerm é um programa de terminal "
		   "para aprendizado de idiomas.\n\n");

	printf("\033[36m");
	printf("J -> Anterior\n");
	printf("K -> Próximo\n");
	printf("I -> Inserir\n");
	printf("H -> Alternar entre Menu\n");
	printf("Q -> Fechar\n");
	printf("\033[0m\n");
}

vector pages_make(const char *const filename, int mode) {
	errors_panic("pages_make (filename)", filename == NULL);
	errors_panic("pages_make (filename == '\\n')", filename[0] == '\n');

	int file = files_make(filename, mode);
	string batch = strings_init(strings_very_large);
	int read_stat = files_read(file, &batch);

	errors_panic("pages_make (batch)", strings_check(&batch));
	errors_warn("pages_make (arquivo não foi lido completamente)", read_stat == 0);
	close(file);
	
	if (batch.size > 1) {
		vector lines = strings_trim(&batch, &linesep);
		strings_free(&batch);
		return lines;
	} 

	vector lines = vectors_init(vectors_string_type);
	strings_free(&batch);
	return lines;
}

string pages_make_line(const string *word) {
	errors_panic("pages_make_line (word)", strings_check_extra(word));

	time_t t = time(NULL);
	struct tm date = *localtime(&t);

	char today[pages_date_len + 1] = "";
	sprintf(today,
		"%02d/%02d/%d",
		date.tm_mday + 1,
		date.tm_mon + 1,
		date.tm_year + 1900
	);

	string temp = strings_init(pages_line_len);
	for (size_t i = 0; i < (word->size - 1) && i < pages_word_len; i++) {
		temp.text[i] = word->text[i];
	}

	temp.text[pages_word_len] = colsep.text[0];
	strncpy(temp.text + pages_word_len + 1, today, pages_date_len);

	ushort score_offset = pages_word_len + pages_date_len + 1;
	temp.text[score_offset] = colsep.text[0];

	ushort note_offset = score_offset + pages_score_len + 1;
	temp.text[note_offset] = colsep.text[0];

	ushort line_offset = note_offset + pages_note_len + 1;
	temp.text[line_offset] = linesep.text[0];

	return temp;
}

#endif
