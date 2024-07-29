#include <stdlib.h>

#include "../lib/strings.h"
#include "../lib/strings.c"

const string colsep = {.text="=", .size=2};
const string linesep = {.text="\n", .size=2};

int main() {
	int asserts_successful = 0;
	int asserts_total = 0;

	string batch = strings_lit("Aal=28/07/2024==Exemplo");
	vector indexes = strings_find(&batch, &colsep);

	asserts_successful += errors_assert("indexes.size == 3", indexes.size == 3);
	asserts_total++;

	vector tokens = vectors_init(vectors_string_virtual_type);
	strings_trim_virtual(&batch, &colsep, &tokens);

	asserts_successful += errors_assert("tokens.size == 4", tokens.size == 4);
	asserts_total++;

	printf("Testes que sucederam: %d/%d\n", asserts_successful, asserts_total);

	vectors_free(&indexes);
	vectors_free(&tokens);

	return EXIT_SUCCESS;
}
