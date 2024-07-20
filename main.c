/*****************************************
 *	- list of german words;
 *	- simple example of use;
 *	- how confortable I am with the word;
 *	- space for typing my examples;
 *	- a currently-trying-to-memorize list.
 ******************************************/

#include "lib/screens.c"
#include "lib/strings.h"
#include "lib/strings.c"
#include "lib/files.h"
#include "lib/files.c"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	screens_raw();
	#define max_lines 5

	char buf[strings_small] = "";
	string buffer = {.text=buf, .size=1, .capacity=strings_small};

	const string linesep = {.text="\n", .size=2};
	const string colsep = {.text=";", .size=2};

	int file = open("./state/dictionary.csv", O_RDONLY);
	errors_panic("main (file == -1)", file == -1);

    vector lines = vectors_init(vectors_string_virtual_type);
    vector cols = vectors_init(vectors_string_virtual_type);

	while (true) {
		bool has_ended = files_read(file, &buffer);
		if (has_ended == true) { 
			printf(colors_success("programa finalizou"));
			break; 
		}

		bool has_error = files_sync(file, &buffer, &linesep, max_lines);
		if (has_error) { break; }

		strings_trim_virtual(&buffer, &linesep, &lines);
		size_t i = 0;
		while (i < lines.size && i < max_lines) {
			system("clear");

			string_virtual line = ((string_virtual *)lines.data)[i];
			strings_trim_virtual((string *)&line, &colsep, &cols);

			for (size_t j = 0; j < cols.size; j++) {
				strings_print((string *)((string_virtual *)cols.data+j));
			}

			printf("\n");
			char opt = '\0';
			read(STDIN_FILENO, &opt, 1);

			if (i > 0 && opt == 'j') {
				i--;
			} else if (i < lines.size && i < max_lines && opt == 'k') {
				i++;
			} else if (opt == 'q') {
				goto quit;
			}
		}
	}
	
	quit:

	system("clear");
	vectors_free(&cols);
	vectors_free(&lines);
	close(file);
	screens_canonical();

	return 0;
}
