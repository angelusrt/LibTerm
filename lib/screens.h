#ifndef screens_h
#define screens_h

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void screens_canonical(void);

void screens_raw(void);

#endif
