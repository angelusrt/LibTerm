#ifndef screens_h
#define screens_h

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define screens_clear_mode true

void screens_canonical(void);

void screens_raw(void);

void screens_clear(void);

#endif
