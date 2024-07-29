#ifndef pages_c
#define pages_c

#include "pages.h"

const string colsep = {.text="=", .size=2};
const string linesep = {.text="\n", .size=2};

void pages_find_columns(page *p, vector *cols) {
	errors_panic("pages_find_columns (p)", p == NULL);
	errors_panic("pages_find_columns (cols)", cols == NULL);
	errors_panic("pages_find_columns (p.line_curr > p.lines.size)", (size_t)p->line_curr > p->lines.size);
	errors_panic("pages_find_columns (p.line_curr < 0)", (size_t)p->line_curr < 0);

	string_virtual line = ((string_virtual *)p->lines.data)[p->line_curr];
	strings_trim_virtual((string *)&line, &colsep, cols);
}

void pages_free(page *p) {
	if (p == NULL) return;

	close(p->file);
	strings_free(&p->batch);
	vectors_free(&p->lines);
	vectors_free(&p->pages);

	p = NULL;
}

string pages_make_line(string *word) {
	errors_panic("pages_make_line (word)", word == NULL);
	errors_panic("pages_make_line (word.size <= 1)", word->size <= 1);

	time_t t = time(NULL);
	struct tm date = *localtime(&t);

	string today = strings_init(pages_date_len+1);
	sprintf(today.text, "%02d/%02d/%d", date.tm_mday + 1, date.tm_mon + 1, date.tm_year + 1900);

	string temp = strings_init(pages_line_len);
	for (size_t i = 0; i < (word->size - 1) && i < pages_word_len; i++) {
		temp.text[i] = word->text[i];
	}

	temp.text[pages_word_len] = colsep.text[0];
	strncpy(temp.text + pages_word_len + 1, today.text, pages_date_len);
	strings_free(&today);

	ushort score_offset = pages_word_len + pages_date_len + 1;
	temp.text[score_offset] = colsep.text[0];

	ushort note_offset = score_offset + pages_score_len + 1;
	temp.text[note_offset] = colsep.text[0];

	ushort line_offset = note_offset + pages_note_len + 1;
	temp.text[line_offset] = linesep.text[0];

	return temp;
}

void pages_print_header(const page *p, const string *s) {
	errors_panic("pages_print_dictionary_header (p)", p == NULL);

	printf("\033[s\033[0;0H");

	if (p->page_type == pages_dictionary_type) {
		printf("\033[31mLinTerm | Dicionário \033[0m| ");
	} else {
		printf("\033[31mLinTerm | Notas \033[0m| ");
	}

	printf("Item: %hu/%hu ", p->line_curr + 1, p->line_last + 1);
	printf("Leva: %hu/%hu ", p->batch_curr + 1, p->batch_last + 1);
	strings_print_no_panic(s);
	printf("\033[u\033[1A\n\n");
}

string pages_log(const page_type p) {
	switch (p) {
		case pages_menu_type: return strings_lit("menu");
		case pages_dictionary_type: return strings_lit("dictionary");
		case pages_notes_type: return strings_lit("notes");
	}

	return strings_lit("not-found");
}

#endif
