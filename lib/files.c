#ifndef files_c
#define files_c
#pragma once

#include "files.h"
#include "errors.c"
#include "strings.c"
#include "vectors.c"

bool files_read(int f, string *buffer) {
	errors_panic("files_read (buffer)", buffer == NULL);
	errors_panic("files_read (buffer.capacity < strings_min)", buffer->capacity < strings_min);

	buffer->text[0] = '\0';
	int bytes_read = read(f, buffer->text, buffer->capacity);
	buffer->text[buffer->capacity - 1] = '\0';
	buffer->size = bytes_read;

	if (bytes_read == 0) {
		buffer->size = strlen(buffer->text) + 1;
		buffer->text[buffer->size - 1] = '\0';

		return 1;
	} else if (bytes_read == -1) {
		printf(colors_warn("files_read (read failed)"));
		buffer->size = 1;
		buffer->text[0] = '\0';

		return 1;
	} else if ((size_t)bytes_read < buffer->capacity){
		buffer->text[bytes_read + 1] = '\0';
	}

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
