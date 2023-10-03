#ifndef DTTERMINALUTIL
#define DTTERMINALUTIL

#include <termios.h>

int setCbreak(int fd, struct termios *prevTermios);

#endif
