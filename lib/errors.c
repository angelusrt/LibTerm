#ifndef errors_c
#define errors_c

#include "errors.h"

bool errors_assert(char *message, bool statement){
	if (!statement) {
		printf(colors_error("o pressuposto '%s' falhou!\n"), message);
		return true;
	}

	printf(colors_success("o pressuposto '%s' funcionou!\n"), message);
	return false;
}

void errors_panic(char *message, bool statement) {
	if (statement) {
		printf(colors_error("%s"), message);
		exit(1);
	}
}

bool errors_warn(char *message, bool statement) {
	if (statement) {
		printf(colors_warn("%s"), message);

		return true;
	}

	return false;
}

#endif
