#include "../lib/vectors.h"
#include "../lib/vectors.c"
#include "../lib/strings.h"
#include "../lib/strings.c"

#include "../pages/pages.h"
#include "../pages/pages.c"

#include "../pages/dictionaries.h"
#include "../pages/dictionaries.c"

int main () {
	vector dictionary = pages_make("../state/dictionary.csv", O_RDONLY);
	vectors_sort(&dictionary, (compfunc)dictionaries_compare_frequency);

	for (size_t i = 0; i < dictionary.size; i++) {
		string s = ((string *)dictionary.data)[i];
		strings_println(&s);
	}

	vectors_free(&dictionary);
	return 0;
}
