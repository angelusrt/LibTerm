#ifndef files_c
#define files_c
#pragma once

#include "files.h"
#include "errors.c"
#include "strings.c"
#include "vectors.c"

int files_make(const char *filename, int mode) {
	errors_panic("files_make (filename)", filename == NULL);
	errors_panic("files_make (filename == '\\0')", filename[0] == '\0');

	int file = open(filename, mode);
	errors_panic("files_make (file == -1)", file == -1);

	return file;
}

int files_read(int f, string *buffer) {
	errors_panic("files_read (buffer)", strings_check(buffer));

	buffer->text[0] = '\0';
	int bytes_read = read(f, buffer->text, buffer->capacity);
	buffer->text[buffer->capacity - 1] = '\0';

	//TODO?: bytes_read may be -1 
	buffer->size = bytes_read;

	if (bytes_read == 0) {
		buffer->size = strlen(buffer->text) + 1;
		buffer->text[buffer->size - 1] = '\0';

		return 1;
	} else if (bytes_read == -1) {
		printf(colors_warn("files_read (read failed)"));
		buffer->size = 1;
		buffer->text[0] = '\0';

		return -1;
	} else if ((size_t)bytes_read < buffer->capacity){
		buffer->text[bytes_read + 1] = '\0';
		return 1;
	}

	//TODO: just return bytes_read
	return 0;
}

bool files_sync(int f, string *buffer, const string *sep, size_t index) {
	vector indexes = strings_find(buffer, sep);

	if (indexes.size >= index) {
		int diff = buffer->size - ((size_t *)indexes.data)[index - 1];
		bool seek_error = lseek(f, -diff + 1, SEEK_CUR) == -1;

		if(seek_error) { 
			printf(colors_warn("files_sync (lseek == -1)"));

			vectors_free(&indexes);
			return 1; 
		}
	}

	vectors_free(&indexes);
	return 0; 
}

#endif
