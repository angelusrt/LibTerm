#ifndef states_h
#define states_h

#include <sys/types.h>
#include <sys/cdefs.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "pages.h"
#include "pages.c"
#include "utils.h"
#include "utils.c"

#include "../lib/vectors.h"
#include "../lib/vectors.c"
#include "../lib/strings.h"
#include "../lib/strings.c"
#include "../lib/files.h"
#include "../lib/files.c"
#include "../lib/screens.h"
#include "../lib/screens.c"

typedef enum _state_type_struct {
	states_init_type,
	states_listen_type,
	states_quit_type,

	states_next_type,
	states_previous_type,

	states_read_type,
	states_prepare_print_type,
	states_print_type,
	states_add_type,
	states_insert_type,

	states_menu_type,
	states_notes_type,
} state_type;

typedef struct _state_struct {
	page *page;
	page_type page_type;
	state_type action;
} state;

void states_controller(state *s, page *ps[]);

#endif
