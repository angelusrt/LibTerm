#include <fcntl.h>
#include <string.h>
#include <unistd.h>

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

	vector dict_lines = pages_make("./state/dictionary.csv", O_RDONLY);
	string *dicts = (string *)dict_lines.data;
	dictionaries_status dict_stats = 0;
	size_t dict_cursor = 0;

	vector note_lines = pages_make("./state/apprentice.csv", O_RDONLY);
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

			dictionaries_print(&dicts[*cursor], *cursor, lines->size, &note, dict_stats);
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
		} else if (option == 'a' && page == 0) {
			string_virtual note = {.size=0};
			long note_index = notes_find(&note_lines, &dicts[*cursor], &note);

			if (note_index < 0) {
				int file = files_make("./state/apprentice.csv", O_RDWR);
				int add_stat = notes_add(file, &dicts[*cursor]);

				if (add_stat == 1) {
					vectors_free(&note_lines);

					note_lines = pages_make("./state/apprentice.csv", O_RDONLY);
					notes = (string *)note_lines.data;
					dict_stats = dictionaries_note_added_status;
				} else {
					dict_stats = dictionaries_note_not_added_status;
				}
				close(file);
			} else {
				dict_stats = dictionaries_note_exists_status;
			}

			redraw = true;
		} else if (option == 'i' && page == 0) {
			string_virtual note = {.size=0};
			long note_index = notes_find(&note_lines, &dicts[*cursor], &note);

			if (note_index >= 0) {
				int file = files_make("./state/apprentice.csv", O_RDWR);
				int insert_stats = notes_insert(file, note_index);

				if (insert_stats != -1) {
					vectors_free(&note_lines);
					note_lines = pages_make("./state/apprentice.csv", O_RDONLY);
					notes = (string *)note_lines.data;

					dict_stats = dictionaries_note_inserted_status;
				} else {
					dict_stats = dictionaries_note_not_inserted_status;
				}

				close(file);
			} else {
				dict_stats = dictionaries_note_not_added_status;
			}
		} else if (option == 'i' && page == 1) {
			int file = files_make("./state/apprentice.csv", O_RDWR);
			int insert_stats = notes_insert(file, *cursor);

			if (insert_stats != -1) {
				vectors_free(&note_lines);
				note_lines = pages_make("./state/apprentice.csv", O_RDONLY);
				notes = (string *)note_lines.data;

				notes_stats = notes_note_inserted_status;
			} else {
				notes_stats = notes_note_not_inserted_status;
			}

			close(file);
		} else {
			redraw = false;
		}
	}

	vectors_free(&dict_lines);
	vectors_free(&note_lines);
	screens_canonical();

	return 0;
}
