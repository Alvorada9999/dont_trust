#include <termios.h>
#include <stdlib.h>

int setCbreak(int fd, struct termios *prevTermios) {
  struct termios t;
  if (tcgetattr(fd, &t) == -1)
    return -3;
  if (prevTermios != NULL)
    *prevTermios = t;
  t.c_lflag &= ~(ICANON | ECHO);
  t.c_lflag |= ISIG;
  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;
  if (tcsetattr(fd, TCSAFLUSH, &t) == -1)
    return -3;
  return 0;
}
