#ifndef DTTERMINALUTIL
#define DTTERMINALUTIL

#define NULLBYTE 0
#define DEL 127
#define LINEFEED 10
#define ESPACE 32

#include <termios.h>

int setCbreak(int fd, struct termios *prevTermios);
void resetTerminal(int fd, struct termios *termiosAttr);

#endif
