#ifndef screens_c
#define screens_c
#pragma once

#include "screens.h"

struct termios orig_termios;

void screens_canonical(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void screens_raw(void) {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(screens_canonical);

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void screens_clear(void) {
	#if screens_clear_mode
		system("clear");
	#endif
}

#endif
