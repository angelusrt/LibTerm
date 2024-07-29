#include <fcntl.h>
#include <unistd.h>

#include "lib/screens.c"
#include "lib/strings.h"
#include "lib/strings.c"
#include "src/states.h"
#include "src/states.c"

int main() {
	//state
	screens_raw();

	page dictionary = {
		.file=files_make("./state/dictionary.csv", O_RDONLY),
		.batch=strings_init(strings_small),
		.batch_curr=-1, 
		.batch_last=0, 
		.lines=vectors_init(vectors_string_virtual_type),
		.line_last=0, 
		.line_curr=0,
		.pages = vectors_init(vectors_size_type),
		.page_type = pages_dictionary_type
	};

	page learning = {
		.file=files_make("./state/apprentice.csv", O_RDWR),
		.batch=strings_init(strings_small),
		.batch_curr=-1, 
		.batch_last=0, 
		.lines=vectors_init(vectors_string_virtual_type),
		.line_last=0, 
		.line_curr=0,
		.pages = vectors_init(vectors_size_type),
		.page_type = pages_notes_type
	};

	vectors_push(&dictionary.pages, 0);
	vectors_push(&learning.pages, 0);

	page *pages[] = { &dictionary, &learning };
	state status = { .action=states_init_type, .page=&dictionary, .page_type=pages_dictionary_type };

	//listener
	while (status.action != states_quit_type) {
		states_controller(&status, pages);

		char option = '\0';
		read(STDIN_FILENO, &option, 1);

		switch (option) {
		case 'k': status.action = states_next_type; break;
		case 'j': status.action = states_previous_type; break;
		case 'a': status.action = states_add_type; break;
		case 'i': status.action = states_insert_type; break;
		case 'h': status.action = states_menu_type; break;
		case 'n': status.action = states_notes_type; break;
		case 'q': status.action = states_quit_type; break;
		default: status.action  = states_listen_type; break;
		}
	}

	//cleanup
	pages_free(&dictionary);
	pages_free(&learning);
	screens_canonical();

	return 0;
}
