#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "lib/vectors.h"
#include "lib/vectors.c"
#include "lib/files.h"
#include "lib/files.c"
#include "lib/screens.h"
#include "lib/screens.c"

#include "pages/dictionaries.h"
#include "pages/dictionaries.c"
#include "pages/notes.h"
#include "pages/notes.c"
#include "pages/pages.h"
#include "pages/pages.c"

int main() {
	//state
	screens_raw();

	vector dict_lines = pages_make(dictionaries_filename, O_RDONLY);
	string *dicts = (string *)dict_lines.data;
	dictionaries_status dict_stats = 0;
	size_t dict_cursor = 0;
	dictionaries_sorting dict_sort = dictionaries_not_sort;

	vector note_lines = pages_make(notes_filename, O_RDONLY | O_CREAT);
	string *notes = (string *)note_lines.data;
	notes_status notes_stats = 0;
	size_t note_cursor = 0;

	bool is_menu = false;
	bool redraw = true;

	ushort page = 0;
	char option = '\0';

	vector *lines = &dict_lines;
	size_t *cursor = &dict_cursor;

	while (option != 'q') {
		//drawing
		if (redraw && is_menu) {
			menus_print();
		} else if (redraw && page == 0) {
			string_virtual note = {.size=0};
			notes_find(&note_lines, &dicts[*cursor], &note);
			dictionaries_print(&dicts[*cursor], *cursor, lines->size, &note, dict_stats, dict_sort);
			dict_stats = 0;
		} else if (redraw && page == 1) {
			notes_print(&notes[*cursor], *cursor, lines->size, notes_stats);
			notes_stats = 0;
		}

		//listening
		read(STDIN_FILENO, &option, 1);

		//controller
		if (option == 'k' && *cursor < lines->size - 1) {
			(*cursor)++;
			redraw = true;
		} else if (option == 'j' && *cursor > 0) {
			(*cursor)--;
			redraw = true;
		} else if (option == 'm') {
			is_menu = !is_menu;
			redraw = true;
		} else if (option == 'n' && page == 0) {
			page = 1;
			lines = &note_lines;
			cursor = &note_cursor;
			redraw = true;
		} else if (option == 'n' && page == 1) {
			page = 0;
			lines = &dict_lines;
			cursor = &dict_cursor;
			redraw = true;
		} else if (option == 's' && page == 0) {
			if (dict_sort == dictionaries_word_sort) {
				vectors_sort(&dict_lines, (compfunc)dictionaries_compare_frequency);
				dict_sort = dictionaries_frequency_sort;
			} else {
				vectors_sort(&dict_lines, (compfunc)strings_compare);
				dict_sort = dictionaries_word_sort;
			}

			redraw = true;
		} else if (option == 'a' && page == 0) {
			string_virtual note = {.size=0};
			long note_index = notes_find(&note_lines, &dicts[*cursor], &note);

			dict_stats = dictionaries_note_exists_status;
			if (note_index < 0) {
				int file = files_make(notes_filename, O_RDWR);
				bool add_stat = notes_add(file, &dicts[*cursor]);
				dict_stats = dictionaries_note_not_added_status;

				if (add_stat) {
					notes_update(&note_lines, &notes);
					dict_stats = dictionaries_note_added_status;
				}

				close(file);
			} 

			redraw = true;
		} else if (option == 'i' && page == 0) {
			string_virtual note = {.size=0};
			long note_index = notes_find(&note_lines, &dicts[*cursor], &note);

			dict_stats = dictionaries_note_not_added_status;
			if (note_index >= 0) {
				int file = files_make(notes_filename, O_RDWR);
				int insert_stats = notes_insert(file, note_index);
				dict_stats = dictionaries_note_not_inserted_status;

				if (insert_stats != -1) {
					notes_update(&note_lines, &notes);
					dict_stats = dictionaries_note_inserted_status;
				} 

				close(file);
			} 

			redraw = true;
		} else if (option == 'i' && page == 1) {
			int file = files_make(notes_filename, O_RDWR | O_CREAT);
			int insert_stats = notes_insert(file, *cursor);
			notes_stats = notes_note_not_inserted_status;

			if (insert_stats != -1) {
				notes_update(&note_lines, &notes);
				notes_stats = notes_note_inserted_status;
			} 

			close(file);

			redraw = true;
		} else if (option == 'r' && page == 0) {
			string_virtual note = {.size=0};
			long note_index = notes_find(&note_lines, &dicts[*cursor], &note);
			dict_stats = dictionaries_note_not_added_status;

			if (note_index >= 0) {
				size_t last_note = note_lines.size - 1;
				string_virtual *n = (string_virtual *)&notes[note_index];
				int file = files_make(notes_filename, O_RDWR | O_CREAT);
				bool remove_stat = notes_remove(file, note_index, last_note, n);
				dict_stats = dictionaries_note_not_removed_status;

				if (remove_stat) {
					notes_update(&note_lines, &notes);
					if (note_lines.size > 1 && note_cursor == last_note) {
						note_cursor--;
					}

					if (note_cursor > last_note) {
						note_cursor = last_note;
					}

					dict_stats = dictionaries_note_removed_status;
				}

				close(file);
			}

			redraw = true;
		} else if (option == 'r' && page == 1 && note_lines.size > 0) {
			int file = files_make(notes_filename, O_RDWR | O_CREAT);
			size_t last_note = note_lines.size - 1;
			string_virtual *note = (string_virtual *)&notes[*cursor];
			bool remove_stat = notes_remove(file, *cursor, last_note, note);
			notes_stats = notes_note_not_removed_status;

			if (remove_stat) {
				notes_update(&note_lines, &notes);
				if (note_lines.size > 1 && *cursor == last_note) {
					(*cursor)--;
				}

				if (*cursor > last_note) {
					*cursor = last_note;
				}

				notes_stats = notes_note_removed_status;
			}

			close(file);

			redraw = true;
		} else {
			redraw = false;
		}
	}

	vectors_free(&dict_lines);
	vectors_free(&note_lines);
	screens_canonical();

	return 0;
}
