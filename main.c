/*****************************************
 *	- list of german words;
 *	- simple example of use;
 *	- how confortable I am with the word;
 *	- space for typing my examples;
 *	- a line_currently-trying-to-memorize list.
 ******************************************/

#include "lib/screens.c"
#include "lib/strings.h"
#include "lib/strings.c"
#include "lib/files.h"
#include "lib/files.c"
#include "lib/vectors.c"
#include "lib/vectors.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct _page_struct {
	short batch_curr;
	ushort batch_last;
	short line_curr;
	ushort line_last;
} page;

typedef enum _action_struct {
	actions_next,
	actions_previous,
	actions_listen,
	actions_quit,
	actions_read,
	actions_print,
	actions_help,
} action;

action pages_seek(int file, size_t index) {
	bool seek_status = lseek(file, index, SEEK_SET) == -1;
	if(seek_status) { 
		printf(colors_warn("files_sync (lseek == -1)"));
		return actions_quit;
	}

	return actions_read;
}

int main() {
	screens_raw();

	struct winsize window;
	int io_error = ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
	errors_panic("main (ioctl == -1)", io_error == -1);

	int file = open("./state/dictionary.csv", O_RDONLY);
	errors_panic("main (file == -1)", file == -1);
	bool file_ended = false;

	char buf[strings_small] = "";
	string buffer = {.text=buf, .size=1, .capacity=strings_small};
	const string linesep = {.text="\n", .size=2};
	const string colsep = {.text="=", .size=2};

    vector lines = vectors_init(vectors_string_virtual_type);
    vector cols = vectors_init(vectors_string_virtual_type);

    vector page_indexes = vectors_init(vectors_size_type);
	size_t *page_indexes_data = (size_t *)page_indexes.data;
	vectors_push(&page_indexes, 0);
	action action = actions_next;
	page page = {.batch_curr=-1, .batch_last=0, .line_last=0, .line_curr=0};
	char option = '\0';

	controller:
	switch (action) {
	case actions_next:
		if (page.line_curr < page.line_last) {
			page.line_curr++;
			action = actions_print;
			goto controller;
		} else if (page.line_curr == page.line_last && page.batch_curr < page.batch_last) {
			page.line_curr = 0;
			action = pages_seek(file, page_indexes_data[++page.batch_curr]);
			goto controller;
		} else {
			action = actions_listen;
		}
	break;
	case actions_previous:
		if (page.line_curr > 0) {
			page.line_curr--;
			action = actions_print;
			goto controller;
		} else if (page.line_curr == 0 && page.batch_curr > 0) {
			page.line_curr = -1;
			action = pages_seek(file, page_indexes_data[--page.batch_curr]);
			goto controller;
		} else {
			action = actions_listen;
		}
	break;
	case actions_read:
		for (size_t i = 0; i < buffer.size; i++) { buffer.text[i] = '\0'; }

		file_ended = files_read(file, &buffer);
		vector line_indexes = strings_find(&buffer, &linesep);
		size_t *line_indexes_data = (size_t *)line_indexes.data;

		if (file_ended && buffer.size >= 1 && buffer.size < buffer.capacity) {
			buffer.text[buffer.size - 2] = '\n';
			line_indexes.size++;
			page.line_last = line_indexes.size - 1;
		} else if (line_indexes.size > 2) {
			page.line_last = line_indexes.size - 2;
		}

		strings_trim_virtual(&buffer, &linesep, &lines);

		if (line_indexes.size <= 2) {
			page.line_curr = 0;
			page.line_last = 1;
			action = actions_print;
			goto controller;
		}

		if (page.line_curr == -1) {
			page.line_curr = page.line_last;
		}

		if (page.batch_curr == page.batch_last && !file_ended) {
			size_t page_last = page_indexes_data[page_indexes.size - 1];

			size_t i = line_indexes.size - 2;
			size_t page_next = (page_last + line_indexes_data[i] + 1);
			vectors_push(&page_indexes, (void *)page_next);

			page.batch_last++;
		}

		vectors_free(&line_indexes);

		action = actions_print;
		goto controller;
	break;
	case actions_print:
		system("clear");

		string_virtual line = ((string_virtual *)lines.data)[page.line_curr];
		strings_trim_virtual((string *)&line, &colsep, &cols);

		printf("\033[31mLinTerm | Dicionário \033[0m");
		printf("| ");
		printf("Item: %hu/%hu ", page.line_curr+1, page.line_last+1);
		printf("Leva: %hu/%hu\n", page.batch_curr+1, page.batch_last+1);

		printf("\033[31m");
		for (size_t i = 0; i < window.ws_col; i++) {
			printf("─");
		}
		printf("\033[0m\n\n");

		if (cols.size >= 5) { 
			string_virtual *cols_data = (string_virtual *)cols.data;

			printf("\033[1m");
			strings_print_no_panic((string *)(cols_data+0));
			printf(", ");

			strings_print_no_panic((string *)(cols_data+2));
			if (cols_data[2].size > 1) { printf(" "); }

			printf("(");
			strings_print_no_panic((string *)(cols_data+3));
			printf(")\n\n");
			printf("\033[0m");

			printf("Frequência: ");
			if (cols_data[1].size > 1) {
				#define max_freq 5

				ushort freq = cols_data[1].text[0] - '0';
				if (freq > 5) { freq = 0; }

				for (size_t i = 0; i < freq; i++) {
					printf("▣");
				}
				for (size_t i = freq; i < max_freq; i++) {
					printf("□");
				}
			}
			printf("\n\n");

			printf("Definição: ");
			strings_print_no_panic((string *)(cols_data+4));
			printf("\n\n");

			printf("Exemplo: ");
			strings_print_no_panic((string *)(cols_data+5));
			printf("\n\n");
		}

	break;
	case actions_help:
		system("clear");
		printf("\033[31mLinTerm | Menu\033[0m\n\n");
		printf("LinTerm é um programa de terminal para aprendizado de idiomas.\n\n");

		printf("\033[36m");
		printf("J -> Anterior\n");
		printf("K -> Próximo\n");
		printf("I -> Inserir\n");
		printf("H -> Alternar entre Menu\n");
		printf("Q -> Fechar\n");
		printf("\033[0m\n");

	case actions_listen:
	break;
	case actions_quit:
	break;
	}

	while (action != actions_quit) {
		read(STDIN_FILENO, &option, 1);

		switch (option) {
		case 'j':
			action = actions_previous;
		break;
		case 'k':
			action = actions_next;
		break;
		case 'q':
			action = actions_quit;
		break;
		case 'h':
			action = action == actions_help ? actions_print : actions_help;
		break;
		default:
			action = actions_listen;
		break;
		}

		goto controller;
	}

	system("clear");
	vectors_free(&cols);
	vectors_free(&lines);
	close(file);
	screens_canonical();

	return 0;
}
