#ifndef errors_c
#define errors_c

#include "errors.h"

//returns true if assert was successfull otherwise false
bool errors_assert(char *message, bool statement){
	if (!statement) {
		printf(colors_error("o pressuposto '%s' falhou!\n"), message);
		return false;
	}

	printf(colors_success("o pressuposto '%s' funcionou!\n"), message);
	return true;
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

void errors_debug(const char *const message, ...) {
    errors_panic("errors_debug (message)", message == NULL);
    errors_panic("errors_debug (message < 1)", strlen(message) < 1);

    va_list args;
    va_start(args, message);

    vprintf(message, args);
    va_end(args);
}

#endif
