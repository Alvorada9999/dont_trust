#include "terminal.h"
#include <termios.h>
#include <unistd.h>

void resetTerminal(int fd, struct termios *termiosAttr) {
  tcsetattr(fd, TCSANOW, termiosAttr);
}
