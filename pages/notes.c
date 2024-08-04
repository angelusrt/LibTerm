#ifndef notes_c
#define notes_c

#include "notes.h"

void notes_print(const string *line, size_t current, size_t total, notes_status notes_stat) {
	screens_clear();

	if (total == 0) {
		printf("\033[31mLinTerm | Notas \033[0m| Item: 0/0\n\n");
		return;
	}

	printf("\033[31mLinTerm | Notas \033[0m| Item: %ld/%ld", current + 1, total);
	errors_panic("notes_print (line)", strings_check_extra(line));

	switch (notes_stat) {
		case notes_note_inserted_status:
			printf("| \033[32mNota inserida\033[0m");
			break;
		case notes_note_not_inserted_status:
			printf("| \033[31mNota não inserida\033[0m");
			break;
	}
	printf("\n\n");

	vector columns = vectors_init(vectors_string_virtual_type);
	strings_trim_virtual(line, &colsep, &columns);
	string_virtual *cols = (string_virtual *)columns.data;

	printf("\033[1m");
	if (columns.size > 0) {
		strings_print_no_panic((string *)(cols+0));
	}

	if (columns.size > 1 && cols+1 != NULL && cols[1].size > 2 && strlen(cols[1].text) > 2) {
		printf(", ");
		strings_print_no_panic((string *)(cols+1));
	} 

	if (columns.size > 2 && cols+2 != NULL && cols[2].size > 2 && strlen(cols[2].text) > 2) {
		printf(", ");
		strings_print_no_panic((string *)(cols+2));
	}

	printf("\033[0m\n\n");
	printf("Nota: \n");
	if (columns.size > 3) {
		strings_print_no_panic((string *)(cols+3));
		printf("\n");
	}

	vectors_free(&columns);
}

long notes_find(const vector *note_lines, const string *entry, string_virtual *note) {
	errors_panic("notes_note (note_lines)", vectors_check(note_lines));
	errors_panic("notes_note (entry)", strings_check_extra(entry));
	errors_panic("notes_note (entry)", strings_check_extra(entry));

	string *notes = (string *)note_lines->data;

	vector indexes = strings_find((string *)entry, &colsep);
	if (indexes.size < 1) goto not_found;

	for (size_t i = 0; i < note_lines->size; i++) {
		int cmp = strncmp(notes[i].text, entry->text, ((size_t *)indexes.data)[0]);

		if (cmp == 0) {
			vectors_free(&indexes);
			note->text = notes[i].text + pages_note_offset;
			note->size = pages_note_len;

			return i;
		}
	}

	not_found:
	vectors_free(&indexes);
	return -1;
}

bool notes_add(int file, const string *entry) {
	errors_panic("notes_add (entry)", strings_check_extra(entry));

	vector indexes = strings_find(entry, &colsep);
	if (indexes.size < 1) goto error_0;

	string_virtual word = {.text=entry->text, .size=((size_t *)indexes.data)[0]+1};
	string temp = pages_make_line((string *)&word);

	int seek_stat = lseek(file, 0, SEEK_END);
	if (seek_stat == -1) goto error_1;

	int write_stat = write(file, temp.text, pages_line_len);
	if (write_stat == -1) goto error_1;

	seek_stat = lseek(file, 0, SEEK_SET);
	if (seek_stat == -1) goto error_1;

	strings_free(&temp);
	vectors_free(&indexes);
	return 1;

	error_1:
	strings_free(&temp);
	error_0:
	vectors_free(&indexes);
	return 0;
}

int notes_insert(int file, size_t index) {
	printf("\033[33mInserir Nota:\033[0m\n");
	screens_canonical();

	string buffer = strings_init(pages_note_len + 1);
	long read_stat = read(STDIN_FILENO, buffer.text, buffer.capacity);
	if (read_stat == -1) goto error_0;

	char *new_line = strchr(buffer.text, '\n');
	if (new_line) { *new_line = '\0'; }

	#define offset(i) (pages_line_len * i) + pages_word_len + 1 + \
						pages_date_len + 1 + pages_score_len + 1

	long seek_stat = lseek(file, offset(index), SEEK_SET);
	if (seek_stat == -1) goto error_0;

	long write_stat = write(file, buffer.text, pages_note_len);
	if (write_stat == -1) goto error_0;

	size_t buffer_size = strlen(buffer.text);
	strings_free(&buffer);
	screens_raw();
	return buffer_size <= pages_note_len;

	error_0:
	strings_free(&buffer);
	screens_raw();
	return -1;
}

#endif
