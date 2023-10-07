#ifndef DTTERMINALUTIL
#define DTTERMINALUTIL

#define DEL 127
#define LINEFEED 10

#include <termios.h>

int setCbreak(int fd, struct termios *prevTermios);
void resetTerminal(int fd, struct termios *termiosAttr);

#endif
