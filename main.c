#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lib/files.c"
#include "lib/files.h"
#include "lib/screens.c"
#include "lib/screens.h"
#include "lib/vectors.c"
#include "lib/vectors.h"

#include "pages/dictionaries.c"
#include "pages/dictionaries.h"
#include "pages/notes.c"
#include "pages/notes.h"
#include "pages/pages.c"
#include "pages/pages.h"
#include "pages/algos.c"
#include "pages/algos.h"

int main() {
    // state
    screens_raw();


    vector dict_lines = pages_make(dictionaries_filename, O_RDONLY);
	vectors_sort(&dict_lines, (compfunc)strings_compare);

    vector dict_lines_filtered = vectors_init(vectors_string_type);
    dictionaries_sorting dict_sort = dictionaries_word_sort;
    dictionaries_status dict_stats = 0;
    size_t dict_cursor = 0;

    vector note_lines = pages_make(notes_filename, O_RDONLY | O_CREAT);
    vector note_lines_filtered = vectors_init(vectors_string_type);
    string *notes = (string *)note_lines.data;
    notes_status notes_stats = 0;
    size_t note_cursor = 0;

	vector dict_parameters = algos_parameterize(&dict_lines, dictionaries_filename);
	algo al = algos_make(&note_lines, &dict_parameters);
	vector resemblance = vectors_init(vectors_double_type);
    size_t al_cursor = 0;

	errors_panic("main (#dict_lines != #dict_parameters)", dict_lines.size != dict_parameters.size);


    bool is_menu = false;
    bool is_algo_toggled = false;
    bool is_algo_enabled = true;
	bool redraw = true;

    ushort page = 0;
    char option = '\0';

    vector *lines = &dict_lines;
    string *lines_data = (string *)lines->data;
    size_t *cursor = &dict_cursor;


    while (option != 'q') {
        // drawing
        if (redraw && is_menu) {
            menus_print();
        } else if (redraw && page == 0) {
            string_virtual note = {.size = 0};
            long index = notes_find(&note_lines, &lines_data[*cursor], &note);

			dictionaries_print(&lines_data[*cursor], *cursor, lines->size, dict_stats, dict_sort);
			if (index >= 0 && (notes+index) != NULL) {
				dictionaries_print_note(notes+index);
			}

			if (is_algo_enabled) {
				if (is_algo_toggled) {
					algos_print(&((vector *)dict_parameters.data)[*cursor]);
				}

				algos_compare(&al, &((vector *)dict_parameters.data)[*cursor], &resemblance); 
				algos_predict(&al.category, &resemblance);
			}

            dict_stats = 0;
        } else if (redraw && page == 1) {
            notes_print(&lines_data[*cursor], *cursor, lines->size, notes_stats);
            notes_stats = 0;
        } else if (redraw && page == 2) {
			algos_print2(&al, *cursor);
		}

        // listening
        read(STDIN_FILENO, &option, 1);

        // controller
		redraw = true;
        if (option == 'k' && *cursor < lines->size - 1) {
            (*cursor)++;
        } else if (option == 'j' && *cursor > 0) {
            (*cursor)--;
        } else if (option == 'l' && *cursor != lines->size - 1) {
            *cursor = lines->size - 1;
        } else if (option == '0' && *cursor != 0) {
            *cursor = 0;
        } else if (option == 'm') {
            is_menu = !is_menu;
		} else if (option == 't' && page == 0 && is_algo_enabled) {
			is_algo_toggled = !is_algo_toggled;
		} else if (option == 'x' && page != 2) {
			page = 2;
            cursor = &al_cursor;
			lines = &al.category;
		} else if (option == 'x' && page == 2) {
			page = 0;
            cursor = &dict_cursor;
			lines = &dict_lines;
        } else if (option == 'n' && page == 0) {
            page = 1;
            cursor = &note_cursor;

			if (note_lines_filtered.size == 0) {
				lines = &note_lines;
				lines_data = (string *)note_lines.data;
			} else {
				lines = &note_lines_filtered;
				lines_data = (string *)note_lines_filtered.data;
			}
        } else if (option == 'n' && page == 1) {
            page = 0;
            cursor = &dict_cursor;

			if (dict_lines_filtered.size == 0) {
				lines = &dict_lines;
				lines_data = (string *)dict_lines.data;
			} else {
				lines = &dict_lines_filtered;
				lines_data = (string *)dict_lines_filtered.data;
			}
        } else if (option == 's' && page == 0 && dict_sort == dictionaries_word_sort) {
			vectors_sort(&dict_lines, (compfunc)dictionaries_compare_frequency);
			is_algo_enabled = false;
			dict_sort = dictionaries_frequency_sort;
        } else if (option == 's' && page == 0 && dict_sort != dictionaries_word_sort) {
			vectors_sort(&dict_lines, (compfunc)strings_compare);
			if (dict_lines_filtered.size == 0) is_algo_enabled = true;
			dict_sort = dictionaries_word_sort;
        } else if (option == 'a' && page == 0) {
            string_virtual note = {.size = 0};
            long note_index = notes_find(&note_lines, &lines_data[*cursor], &note);

            dict_stats = dictionaries_note_exists_status;
            if (note_index < 0) {
                int file = files_make(notes_filename, O_RDWR);
                bool add_stat = notes_add(file, &lines_data[*cursor]);
                dict_stats = dictionaries_note_not_added_status;

                if (add_stat) {
                    notes_update(&note_lines, &notes);
                    dict_stats = dictionaries_note_added_status;
                }

                close(file);
            }
        } else if (option == 'i' && page == 0) {
            string_virtual note = {.size = 0};
            long note_index = notes_find(&note_lines, &lines_data[*cursor], &note);

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
        } else if (option == 'i' && page == 1) {
            int file = files_make(notes_filename, O_RDWR | O_CREAT);
            int insert_stats = notes_insert(file, *cursor);
            notes_stats = notes_note_not_inserted_status;

            if (insert_stats != -1) {
                notes_update(&note_lines, &notes);
                notes_stats = notes_note_inserted_status;
            }

            close(file);
		} else if (option == 'c' && page == 0) {
            string_virtual note = {.size = 0};
            long note_index = notes_find(&note_lines, &lines_data[*cursor], &note);

            dict_stats = dictionaries_note_not_added_status;
            if (note_index >= 0) {
                int file = files_make(notes_filename, O_RDWR);
                int insert_stats = notes_categorize(file, note_index);
                dict_stats = dictionaries_note_not_categorized_status;

                if (insert_stats != -1) {
                    notes_update(&note_lines, &notes);
                    dict_stats = dictionaries_note_categorized_status;
                }

				algos_free(&al);
				al = algos_make(&note_lines, &dict_parameters);

                close(file);
            }
		} else if (option == 'c' && page == 1) {
            int file = files_make(notes_filename, O_RDWR | O_CREAT);
            int insert_stats = notes_categorize(file, *cursor);
            notes_stats = notes_note_not_categorized_status;

            if (insert_stats != -1) {
                notes_update(&note_lines, &notes);
                notes_stats = notes_note_categorized_status;
            }

			algos_free(&al);
			al = algos_make(&note_lines, &dict_parameters);

            close(file);
        } else if (option == 'f' && page == 0 && dict_lines_filtered.size == 0) {
			string buffer;
			strings_preinit(buffer, pages_word_len);
			pages_search(&buffer);

			dict_stats = dictionaries_filter_not_matched_status;
            if (buffer.size > 0) {
                pages_filter(&dict_lines, &buffer, &dict_lines_filtered);

				if (dict_lines_filtered.size > 0) {
					lines = &dict_lines_filtered;
					lines_data = (string *)dict_lines_filtered.data;
					if (dict_lines_filtered.size > 0 && dict_lines_filtered.size < *cursor) {
						*cursor = dict_lines_filtered.size - 1;
					} else if (dict_lines_filtered.size == 0) {
						*cursor = 0;
					}

					is_algo_enabled = false;
					dict_stats = dictionaries_filter_enabled_status;
				}
            }
        } else if (option == 'f' && page == 0 && dict_lines_filtered.size != 0) {
            lines = &dict_lines;
            lines_data = (string *)dict_lines.data;
			if (dict_sort == dictionaries_word_sort) is_algo_enabled = true;

            vectors_free(&dict_lines_filtered);
            dict_lines_filtered = vectors_init(vectors_string_type);
			dict_stats = dictionaries_filter_disabled_status;
        } else if (option == 'f' && page == 1 && note_lines_filtered.size == 0) {
			string buffer;
			strings_preinit(buffer, pages_word_len);
			pages_search(&buffer);

			notes_stats = notes_filter_not_matched_status;
            if (buffer.size > 0) {
                pages_filter(&note_lines, &buffer, &note_lines_filtered);

				if (note_lines_filtered.size > 0) {
					lines = &note_lines_filtered;
					lines_data = (string *)note_lines_filtered.data;
					if (note_lines_filtered.size > 0 && note_lines_filtered.size < *cursor) {
						*cursor = note_lines_filtered.size - 1;
					} else if (note_lines_filtered.size == 0) {
						*cursor = 0;
					}

					notes_stats = notes_filter_enabled_status;
				}
            }
        } else if (option == 'f' && page == 1 && note_lines_filtered.size > 0) {
            lines = &note_lines;
            lines_data = (string *)note_lines.data;

            vectors_free(&note_lines_filtered);
            note_lines_filtered = vectors_init(vectors_string_type);
			notes_stats = notes_filter_disabled_status;
        } else if (option == 'd' && page == 0) {
            string_virtual note = {.size = 0};
            long note_index = notes_find(&note_lines, &lines_data[*cursor], &note);

            dict_stats = dictionaries_note_not_added_status;
            if (note_index >= 0) {
                int file = files_make(notes_filename, O_RDWR);
                int insert_stats = notes_define(file, note_index);
                dict_stats = dictionaries_note_not_defined_status;

                if (insert_stats != -1) {
                    notes_update(&note_lines, &notes);
                    dict_stats = dictionaries_note_defined_status;
                }

                close(file);
            }
        } else if (option == 'd' && page == 1) {
            int file = files_make(notes_filename, O_RDWR | O_CREAT);
            int insert_stats = notes_define(file, *cursor);
            notes_stats = notes_note_not_defined_status;

            if (insert_stats != -1) {
                notes_update(&note_lines, &notes);
                notes_stats = notes_note_defined_status;
            }

            close(file);
        } else if (option == 'r' && page == 0) {
            string_virtual note = {.size = 0};
            long note_index = notes_find(&note_lines, &lines_data[*cursor], &note);
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
        } else {
            redraw = false;
        }
    }

    vectors_free(&dict_lines);
    vectors_free(&note_lines);
    vectors_free(&dict_lines_filtered);
    vectors_free(&note_lines_filtered);
	vectors_free(&dict_parameters);
	vectors_free(&resemblance);
	algos_free(&al);
    screens_canonical();

    return 0;
}
