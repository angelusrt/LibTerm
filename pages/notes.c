#ifndef notes_c
#define notes_c

#include "notes.h"

void notes_print(const string *line, size_t current, size_t total, notes_status notes_stat) {
	screens_clear();

	if (total == 0) {
		printf("\033[31mLinTerm | Notas \033[0m| Item: 0/0\n\n");
		return;
	}

	printf("\033[31mLinTerm | Notas \033[0m| Item: %ld/%ld ", current + 1, total);
	errors_panic("notes_print (line)", strings_check_extra(line));

	switch (notes_stat) {
	case notes_note_inserted_status:
		printf("| \033[32mNota inserida\033[0m");
	break;
	case notes_note_not_inserted_status:
		printf("| \033[31mNota não inserida\033[0m");
	break;
	case notes_note_removed_status:
		printf("| \033[32mNota removida\033[0m");
	break;
	case notes_note_not_removed_status:
		printf("| \033[32mNota não removida\033[0m");
	break;
	case notes_note_not_defined_status:
		printf("| \033[31mDefinição não adicionada\033[0m");
	break;
	case notes_note_defined_status:
		printf("| \033[32mDefinição adicionada\033[0m");
	break;
	case notes_note_categorized_status:
		printf("| \033[32mCategoria adicionada\033[0m");
	break;
	case notes_note_not_categorized_status:
		printf("| \033[32mCategoria não adicionada\033[0m");
	break;
	case notes_filter_enabled_status:
		printf("| \033[32mFiltro ativado\033[0m");
	break;
	case notes_filter_disabled_status:
		printf("| \033[32mFiltro desativado\033[0m");
	break;
	case notes_filter_not_matched_status:
		printf("| \033[31mNenhuma correspondência\033[0m");
	break;
	}

	printf("\n\n\n");

	vector columns = vectors_init(vectors_string_virtual_type);
	strings_make_trim_virtual(line, &colsep, &columns);
	string_virtual *cols = (string_virtual *)columns.data;

	printf("\033[1m");
	if (columns.size > 0) {
		strings_print_no_panic((string *)(cols+0));
	}

	if (pages_columns_check(2)) {
		printf(" [");
		strings_print_no_panic((string *)(cols+2));
		printf("] ");
	} 

	if (pages_columns_check(3)) {
		printf(" [");
		strings_print_no_panic((string *)(cols+3));
		printf("] ");
	}

	printf("\033[0m\n\n\n");

	printf("Definição \n\n🮌 ");
	if (columns.size > 1) {
		strings_print_no_panic((string *)(cols+1));
		printf("\n\n\n");
	} 

	printf("Categoria \n\n🮌 ");
	if (columns.size > 5) {
		strings_print_no_panic((string *)(cols+5));
		printf("\n\n\n");
	}

	printf("Nota \n\n🮌 ");
	if (columns.size > 4) {
		strings_print_no_panic((string *)(cols+4));
		printf("\n");
	}

	printf("\n\n");

	vectors_free(&columns);
}

long notes_find(const vector *note_lines, const string *entry, string_virtual *note) {
	errors_panic("notes_find (note_lines)", vectors_check(note_lines));
	errors_panic("notes_find (entry)", strings_check_extra(entry));
	errors_panic("notes_find (entry)", strings_check_extra(entry));

	string *notes = (string *)note_lines->data;

	vector indexes = strings_make_find((string *)entry, &colsep);
	if (indexes.size < 1) goto not_found;

	for (size_t i = 0; i < note_lines->size; i++) {
		int cmp = strncmp(notes[i].text, entry->text, ((size_t *)indexes.data)[0]);

		if (cmp == 0) {
			vectors_free(&indexes);
			note->text = notes[i].text + pages_note_offset(0) + 1;
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

	vector indexes = strings_make_find(entry, &colsep);
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

	string buffer; 
	strings_preinit(buffer, pages_note_len + 1);

	long read_stat = read(STDIN_FILENO, buffer.text, buffer.capacity);
	if (read_stat == -1) goto error_0;

	char *new_line = strchr(buffer.text, '\n');
	if (new_line) { *new_line = '\0'; }

	long seek_stat = lseek(file, pages_note_offset(index) + 1, SEEK_SET);
	if (seek_stat == -1) goto error_0;

	long write_stat = write(file, buffer.text, pages_note_len);
	if (write_stat == -1) goto error_0;

	size_t buffer_size = strlen(buffer.text);
	screens_raw();
	return buffer_size <= pages_note_len;

	error_0:
	screens_raw();
	return -1;
}

int notes_define(int file, size_t index) {
	printf("\033[33mInserir Definição:\033[0m\n");
	screens_canonical();

	string buffer; 
	strings_preinit(buffer, pages_definition_len + 1);

	long read_stat = read(STDIN_FILENO, buffer.text, buffer.capacity);
	if (read_stat == -1) goto error_0;

	char *new_line = strchr(buffer.text, '\n');
	if (new_line) { *new_line = '\0'; }

	long seek_stat = lseek(file, pages_definition_offset(index) + 1, SEEK_SET);
	if (seek_stat == -1) goto error_0;

	long write_stat = write(file, buffer.text, pages_definition_len);
	if (write_stat == -1) goto error_0;

	size_t buffer_size = strlen(buffer.text);
	screens_raw();
	return buffer_size <= pages_definition_len;

	error_0:
	screens_raw();
	return -1;
}

int notes_categorize(int file, size_t index) {
	printf("\033[33mInserir Categoria:\033[0m\n");
	screens_canonical();

	string buffer; 
	strings_preinit(buffer, pages_category_len + 1);

	long read_stat = read(STDIN_FILENO, buffer.text, buffer.capacity);
	if (read_stat == -1) goto error_0;

	char *new_line = strchr(buffer.text, '\n');
	if (new_line) { *new_line = '\0'; }

	long seek_stat = lseek(file, pages_category_offset(index) + 1, SEEK_SET);
	if (seek_stat == -1) goto error_0;

	long write_stat = write(file, buffer.text, pages_category_len);
	if (write_stat == -1) goto error_0;

	size_t buffer_size = strlen(buffer.text);
	screens_raw();
	return buffer_size <= pages_category_len;

	error_0:
	screens_raw();
	return -1;
}

bool notes_remove(int file, size_t index, size_t last, string_virtual *note) {
	errors_panic("notes_remove (note)", string_virtuals_check(note));

	//--archiving
	int archive = files_make(archives_filename, O_RDWR | O_CREAT);
	if (archive < 0) goto error_0;

	int seek_stat = lseek(archive, 0, SEEK_END);
	if (seek_stat < 0) goto error_0;

	long write_stat = write(archive, note->text, note->size);
	if (write_stat < 0) goto error_0;

	close(archive);
	//--

	//--substitute it for last line
	if (index < last) {
		string last_buffer; 
		strings_preinit(last_buffer, pages_line_len);

		long seek_stat = lseek(file, (pages_line_len * last), SEEK_SET);
		if (seek_stat < 0) goto error_1;

		//TODO: make read_stat error handling thorough
		long read_stat = read(file, last_buffer.text, last_buffer.capacity);
		if (read_stat < 0) goto error_1;

		seek_stat = lseek(file, (pages_line_len * index), SEEK_SET);
		if (seek_stat < 0) goto error_1;

		long write_stat = write(file, last_buffer.text, last_buffer.capacity);
		if (write_stat < 0) goto error_1;
	}
	//--

	//--delete last line
	seek_stat = lseek(file, (pages_line_len * last), SEEK_SET);
	if (seek_stat < 0) goto error_1;

	int truncate_stat = ftruncate(file, seek_stat);
	if (truncate_stat < 0) goto error_1;
	//--

	return 1;

	error_1:
	return 0;

	error_0:
	close(archive);
	return 0;
}

void notes_update(vector *note_lines, string **notes) {
	vectors_free(note_lines);

	*note_lines = pages_make(notes_filename, O_RDONLY);
	*notes = (string *)note_lines->data;
}

#endif
