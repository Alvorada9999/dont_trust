// This file is part of dont_trust.
// Copyright (C) 2023 Kenedy Henrique Bueno Silva

// dont_trust is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
