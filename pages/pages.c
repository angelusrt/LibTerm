#ifndef pages_c
#define pages_c

#include "pages.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void menus_print() {
	screens_clear();

	printf("\033[31mLinTerm | Menu\033[0m\n\n\n");
	printf("LinTerm é um programa de terminal "
		   "para aprendizado de idiomas.\n\n\n");

	printf("\033[2m");
	printf("Comandos\n\n");
	printf("🮌 J  Anterior\n");
	printf("🮌 K  Próximo\n");
	printf("🮌 I  Inserir Nota\n");
	printf("🮌 A  Adicionar Nota\n");
	printf("🮌 R  Arquivar Nota\n");
	printf("🮌 D  Adicionar Definição\n");
	printf("🮌 C  Adicionar Categoria\n");
	printf("🮌 T  Mostrar parametros\n");
	printf("🮌 M  Alternar entre Menu\n");
	printf("🮌 N  Alternar entre Notas\n");
	printf("🮌 X  Alternar entre Categorias\n");
	printf("🮌 F  filtrar páginas\n");
	printf("🮌 S  Ordenar (alfabeticamente ou por frequência)\n");
	printf("🮌 L  Ir para o final da lista\n");
	printf("🮌 0  Ir para o começo da lista\n");
	printf("🮌 Q  Fechar\n");
	printf("\033[0m\n\n");
}

vector pages_make(const char *const filename, int mode) {
	errors_panic("pages_make (filename)", filename == NULL);
	errors_panic("pages_make (filename == '\\n')", filename[0] == '\n');

	int file = files_make(filename, mode);

	//--get size
	long seek_stat = lseek(file, 0, SEEK_END);
	errors_panic("pages_make (seek_stat < 0)", seek_stat < 0);

	size_t new_size = strings_min;
	while (new_size < (size_t)seek_stat) {
		size_t new_new_size = new_size << 1;
		if (new_new_size < new_size) break;

		new_size = new_new_size;
	}

	seek_stat = lseek(file, 0, SEEK_SET);
	errors_panic("pages_make (seek_stat < 0)", seek_stat < 0);
	//--

	string batch = strings_init(new_size);
	int read_stat = files_read(file, &batch);

	errors_panic("pages_make (batch)", strings_check(&batch));
	errors_warn("pages_make (arquivo não foi lido completamente)", read_stat == 0);
	close(file);
	
	if (batch.size > 1) {
		vector lines = strings_trim(&batch, &linesep);
		strings_free(&batch);
		return lines;
	} 

	vector lines = vectors_init(vectors_string_type);
	strings_free(&batch);
	return lines;
}

string pages_make_line(const string *word) {
	errors_panic("pages_make_line (word)", strings_check_extra(word));

	time_t t = time(NULL);
	struct tm date = *localtime(&t);

	char today[pages_date_len + 1] = "";
	sprintf(today,
		"%02d/%02d/%d",
		date.tm_mday + 1,
		date.tm_mon + 1,
		date.tm_year + 1900
	);

	string temp = strings_init(pages_line_len);
	for (size_t i = 0; i < (word->size - 1) && i < pages_word_len; i++) {
		temp.text[i] = word->text[i];
	}

	temp.text[pages_definition_offset(0)] = colsep.text[0];
	temp.text[pages_date_offset(0)] = colsep.text[0];
	strncpy(temp.text + pages_date_offset(0) + 1, today, pages_date_len);
	temp.text[pages_score_offset(0)] = colsep.text[0];
	temp.text[pages_note_offset(0)] = colsep.text[0];
	temp.text[pages_category_offset(0)] = colsep.text[0];
	temp.text[pages_line_offset(0)] = linesep.text[0];

	return temp;
}

void pages_search(string *buffer) {
	buffer->size = 0;

	printf("\033[33mpesquise: \033[0m\n");

	screens_canonical();
	long read_stat = read(STDIN_FILENO, buffer->text, buffer->capacity);
	screens_raw();

	if (read_stat < 0) return;

	buffer->size = strnlen(buffer->text, buffer->capacity - 1);

	char *buffer_new_line = strchr(buffer->text, '\n');
	if (buffer_new_line[0] == '\n') {
		buffer_new_line[0] = '\0';
	}
}

void pages_filter(const vector *page_lines, const string *word, vector *page_filtered) {
	errors_panic("pages_filter (page_lines)", vectors_check(page_lines));
	errors_panic("pages_filter (page_filtered)", vectors_check(page_filtered));
	errors_panic("pages_filter (word)", strings_check_extra(word));

	string *pages = (string *)page_lines->data;

	for (size_t i = 0; i < page_lines->size; i++) {
		string page = pages[i];
		size_t size = word->size;

		if (page.size < size) {
			size = page.size;
		}

		if (page.size == 0) {
			size = 1;
		}

		int comp = strncmp(page.text, word->text, size - 1);
		if (comp == 0) {
			string new_page = strings_init(page.capacity);
			for (size_t i = 0; i < page.capacity; i++) {
				new_page.text[i] = page.text[i];
			}

			new_page.size = page.size;
			vectors_push(page_filtered, &new_page);
		}
	}
}

#endif
