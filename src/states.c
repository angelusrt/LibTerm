#ifndef states_c
#define states_c

#include "states.h"

__attribute_warn_unused_result__
state_type states_next(state *s) {
	errors_panic("states_next (s)", s == NULL);
	errors_panic("states_next (s.page)", s->page == NULL);

	size_t *pages = (size_t *)s->page->pages.data;

	if (s->page->line_curr < s->page->line_last) {
		s->page->line_curr++;
		return states_prepare_print_type;
	} else if (s->page->line_curr == s->page->line_last && s->page->batch_curr < s->page->batch_last) {
		s->page->line_curr = 0;
		int seek_status = lseek(s->page->file, pages[++s->page->batch_curr], SEEK_SET);

		if(seek_status == -1) { 
			printf(colors_warn("files_sync (lseek == -1)"));
			return states_quit_type;
		} 

		return states_read_type;
	} 

	return states_listen_type;
}

__attribute_warn_unused_result__
state_type states_previous(state *s) {
	errors_panic("states_previous (s)", s == NULL);
	errors_panic("states_previous (s.page)", s->page == NULL);

	size_t *pages = (size_t *)s->page->pages.data;

	if (s->page->line_curr > 0) {
		s->page->line_curr--;
		return states_prepare_print_type;
	} else if (s->page->line_curr == 0 && s->page->batch_curr > 0) {
		s->page->line_curr = -1;
		int seek_status = lseek(s->page->file, pages[--s->page->batch_curr], SEEK_SET);

		if(seek_status == -1) { 
			printf(colors_warn("files_sync (lseek == -1)"));
			return states_quit_type;
		}

		return states_read_type;
	} 

	return states_listen_type;
}

__attribute_warn_unused_result__
state_type states_read(state *s) {
	errors_panic("states_read (s)", s == NULL);
	errors_panic("states_read (s.page)", s->page == NULL);

	for (size_t i = 0; i < s->page->batch.size; i++) { 
		s->page->batch.text[i] = '\0'; 
	}

	bool file_ended = files_read(s->page->file, &s->page->batch);
	vector line_indexes = strings_find(&s->page->batch, &linesep);
	size_t *lines = (size_t *)line_indexes.data;

	if (file_ended && s->page->batch.size >= 1 && s->page->batch.size < s->page->batch.capacity) {
		s->page->batch.text[s->page->batch.size - 2] = '\n';
		line_indexes.size++;
		s->page->line_last = line_indexes.size - 1;
	} else if (line_indexes.size > 2) {
		s->page->line_last = line_indexes.size - 2;
	}

	strings_trim_virtual(&s->page->batch, &linesep, &s->page->lines);

	if (line_indexes.size <= 2) {
		s->page->line_curr = 0;
		s->page->line_last = 1;
		return states_prepare_print_type;
	}

	if (s->page->line_curr == -1) {
		s->page->line_curr = s->page->line_last;
	}

	if (s->page->batch_curr == s->page->batch_last && !file_ended) {
		size_t *pages = (size_t *)s->page->pages.data;
		size_t page_last = pages[s->page->pages.size - 1];

		size_t i = line_indexes.size - 2;
		size_t page_next = (page_last + lines[i] + 1);
		vectors_push(&s->page->pages, (void *)page_next);

		s->page->batch_last++;
	}

	vectors_free(&line_indexes);

	return states_prepare_print_type;
}

__attribute_warn_unused_result__
state_type states_prepare_print(state *s, page *ps[], vector *columns, string *note) {
	errors_panic("states_prepare_print (s)", s == NULL);
	errors_panic("states_prepare_print (s.page)", s->page == NULL);
	errors_panic("states_prepare_print (ps)", ps == NULL);
	errors_panic("states_prepare_print (columns)", columns == NULL);
	errors_panic("states_prepare_print (columns.type != string_virtual)", columns->type != vectors_string_virtual_type);
	errors_panic("states_prepare_print (note)", note == NULL);
	errors_panic("states_prepare_print (note.capacity != note_len)", note->capacity != pages_note_len);

	pages_find_columns(s->page, columns);

	if (s->page->page_type == pages_notes_type) {
		if (columns->size >= 3) {
			return states_print_type;
		} 

		return states_notes_type;
	} 

	if (columns->size < 5) {
		return states_listen_type;
	}

	string_virtual word = ((string_virtual *)columns->data)[0];
	long find_stat = utils_find_line(ps[1]->file, (string *)&word, pages_line_len);

	if (find_stat != -1) {
		#define offset(i) (pages_line_len * i) + pages_word_len + 1 + \
							pages_date_len + 1 + pages_score_len + 1

		//TODO: be thorough with error handling
		lseek(ps[1]->file, offset(find_stat), SEEK_SET);
		read(ps[1]->file, note->text, pages_note_len - 1);

		note->size = strlen(note->text) + 1;
	} else {
		note->size = 0;
	}

	return states_print_type;
}

__attribute_warn_unused_result__
state_type states_print_dictionary(state *s, vector *columns, string *note) {
	errors_panic("states_print (s)", s == NULL);
	errors_panic("states_print (s.page)", s->page == NULL);
	errors_panic("states_print (columns.type != string_virtual)", columns->type != vectors_string_virtual_type);
	errors_panic("states_print (note)", note == NULL);
	errors_panic("states_print (note.capacity != note_len)", note->capacity != pages_note_len);

	string_virtual *cols_data = (string_virtual *)columns->data;

	system("clear");
	pages_print_header(s->page, NULL);

	printf("\033[1m");
	strings_print_no_panic((string *)(cols_data+0));
	printf(", ");

	strings_print_no_panic((string *)(cols_data+2));
	if (cols_data[2].size > 1) { 
		printf(" "); 
	}

	printf("(");
	strings_print_no_panic((string *)(cols_data+3));
	printf(")\n\n");
	printf("\033[0m");

	printf("Frequência: ");
	utils_print_frequency(cols_data+1);
	printf("\n\n");

	printf("Definição: ");
	strings_print_no_panic((string *)(cols_data+4));
	printf("\n\n");

	printf("Exemplo: ");
	strings_print_no_panic((string *)(cols_data+5));
	printf("\n\n");

	if (note->size != 0) {
		printf("Nota:\n");
		printf("%s", note->text);
		printf("\n\n");
	}

	return states_listen_type;
}

__attribute_warn_unused_result__
state_type states_print_notes(state *s, vector *columns) {
	errors_panic("states_print_notes (s)", s == NULL);
	errors_panic("states_print_notes (s.page)", s->page == NULL);
	errors_panic("states_print_notes (columns)", columns == NULL);
	errors_panic("states_print_notes (columns.type != string_virtual)", columns->type != vectors_string_virtual_type);

	string_virtual *cols_data = (string_virtual *)columns->data;

	system("clear");
	pages_print_header(s->page, NULL);

	printf("\033[1m");
	strings_print_no_panic((string *)(cols_data+0));

	if (cols_data+1 != NULL && cols_data[1].size > 2 && strnlen(cols_data[1].text, pages_date_len) > 1) {
		printf(", ");
		strings_print_no_panic((string *)(cols_data+1));
	} 

	if (cols_data+2 != NULL && cols_data[2].size > 2 && strnlen(cols_data[2].text, pages_score_len) > 1) {
		printf(", ");
		strings_print_no_panic((string *)(cols_data+2));
	}

	printf("\033[0m\n\n");
	printf("Nota: \n\n");
	strings_print_no_panic((string *)(cols_data+3));
	printf("\n\n");

	return states_listen_type;
}

__attribute_warn_unused_result__
state_type states_menu() {
	system("clear");

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

	return states_listen_type;
}

__attribute_warn_unused_result__
state_type states_add(state *s, page *ps[], vector *columns) {
	errors_panic("states_add (s)", s == NULL);
	errors_panic("states_add (s.page)", s->page == NULL);
	errors_panic("states_add (columns.type != string_virtual)", columns->type != vectors_string_virtual_type);

	if (s->page->page_type != pages_dictionary_type) {
		return states_listen_type;
	}

	pages_find_columns(s->page, columns);
	if (columns->size <= 1) {
		vectors_free(columns);
		return states_quit_type;
	}

	string_virtual word = ((string_virtual *)columns->data)[0];

	long find_stat = utils_find_line(ps[1]->file, (string *)&word, pages_line_len);
	if (find_stat != -1) {
		string notes_stat = {.text="| \033[33mPalavra já existe\033[0m", .size=30};
		pages_print_header(s->page, &notes_stat);
		return states_listen_type;
	}

	string temp = pages_make_line((string *)&word);

	//TODO: be thorough with errors
	lseek(ps[1]->file, 0, SEEK_END);
	write(ps[1]->file, temp.text, pages_line_len);
	lseek(ps[1]->file, 0, SEEK_SET);

	strings_free(&temp);

	string notes_stat = {.text="| \033[32mPalavra adicionada\033[0m", .size=30};
	pages_print_header(s->page, &notes_stat);

	return states_listen_type;
}

__attribute_warn_unused_result__
state_type states_insert(state *s, page *ps[], vector *columns) {
	errors_panic("states_insert (s)", s == NULL);
	errors_panic("states_insert (s.page)", s->page == NULL);
	errors_panic("states_insert (columns)", columns == NULL);
	errors_panic("states_insert (columns.type != string_virtual)", 
			columns->type != vectors_string_virtual_type);

	pages_find_columns(s->page, columns);
	if (columns->size <= 1) {
		vectors_free(columns);
		return states_listen_type;
	}

	string_virtual *cols_data = (string_virtual *)columns->data;
	long line_stat = utils_find_line(ps[1]->file, (string *)&cols_data[0], pages_line_len);

	if (line_stat == -1) {
		const string str = { .text="| \033[31mPalavra não adicionada (dica: a)\033[0m", .size=45 };
		pages_print_header(s->page, &str);

		return states_listen_type;
	}

	printf("\033[33mInserir Nota:\033[0m\n");
	screens_canonical();

	string buffer = strings_init(pages_note_len + 1);
	read(STDIN_FILENO, buffer.text, buffer.capacity);

	char *new_line = strchr(buffer.text, '\n');
	if (new_line) { *new_line = '\0'; }

	buffer.size = strlen(buffer.text);
	bool is_too_big = buffer.size > pages_note_len;

	string notes_stat = strings_make_format(
			"| \033[33mNota %s\033[0m", 
			is_too_big ? "grande, adicionando cortada" : "adicionada com sucesso"
	);

	pages_print_header(s->page, &notes_stat);
	strings_free(&notes_stat);

	size_t note_offset = pages_word_len + 1 + pages_date_len + 1 + pages_score_len + 1;
	lseek(ps[1]->file, (line_stat * pages_line_len) + note_offset, SEEK_SET);
	write(ps[1]->file, buffer.text, pages_note_len);

	strings_free(&buffer);

	screens_raw();

	return states_prepare_print_type;
}

vector columns;
string note;
int file_log;

__attribute_warn_unused_result__
state_type states_init_global(void) {
	columns = vectors_init(vectors_string_virtual_type);
	note = strings_init(pages_note_len);
	file_log = files_make("log.md", O_WRONLY | O_CREAT);

	return states_next_type;
	//return states_notes_type;
}

__attribute_warn_unused_result__
state_type states_free_global(void) {
	vectors_free(&columns);
	strings_free(&note);
	close(file_log);

	return states_quit_type;
}

__attribute_warn_unused_result__
string states_log(state_type action) {
	switch (action) {
	case states_init_type: return strings_lit("init");
	case states_quit_type: return strings_lit("quit");
	case states_listen_type: return strings_lit("listen");
	case states_next_type: return strings_lit("next");
	case states_previous_type: return strings_lit("previous");
	case states_read_type: return strings_lit("read");
	case states_prepare_print_type: return strings_lit("prepare-print");
	case states_print_type: return strings_lit("print");
	case states_add_type: return strings_lit("add");
	case states_insert_type: return strings_lit("insert");
	case states_notes_type: return strings_lit("notes");
	case states_menu_type: return strings_lit("menu");
	}

	return strings_lit("not_found");
}

void states_controller(state *s, page *ps[]) {
	errors_panic("states_controller (s)", s == NULL);
	errors_panic("states_controller (ps)", ps == NULL);

	switch (s->action) {
	case states_init_type:
		s->action = states_init_global();
		write(file_log, "init dictionary\n", 17);
	break;
	case states_quit_type:
		write(file_log, "quit -\n", 8);
		s->action = states_free_global();
		return;
	break;
	case states_listen_type:
		return;
	break;
	case states_next_type:
		s->action = states_next(s);
	break;
	case states_previous_type:
		s->action = states_previous(s);
	break;
	case states_read_type:
		s->action = states_read(s);
	break;
	case states_prepare_print_type:
		s->action = states_prepare_print(s, ps, &columns, &note);
	break;
	case states_print_type:
		if(s->page->page_type == pages_dictionary_type) {
			s->action = states_print_dictionary(s, &columns, &note);
		} else {
			s->action = states_print_notes(s, &columns);
		}
	break;
	case states_add_type:
		s->action = states_add(s, ps, &columns);
	break;
	case states_insert_type:
		s->action = states_insert(s, ps, &columns);
	break;
	case states_notes_type:
		if (s->page->page_type == pages_dictionary_type) {
			s->page = ps[1];
			s->page_type = pages_notes_type;
		} else {
			s->page = ps[0];
			s->page_type = pages_dictionary_type;
		}

		s->action = states_next_type;
	break;
	case states_menu_type:
		if (s->page_type == pages_menu_type) {
			s->page_type = s->page->page_type;
			s->action = states_print_type;
			break;
		} 

		s->page_type = pages_menu_type;
		s->action = states_menu();
	break;
	}

	string state_log = states_log(s->action);
	string page_log = pages_log(s->page_type);

	write(file_log, state_log.text, state_log.size - 1);
	write(file_log, " ", 1);
	write(file_log, page_log.text, page_log.size - 1);
	write(file_log, "\n", 1);

	states_controller(s, ps);
}

#endif
