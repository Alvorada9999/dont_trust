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

#include <string.h>
#include <stdio.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "common.h"

void renderCurrentlyBeingWrittenMessage(char *inputBufer, uint16_t inputBufferSize) {
  if(inputBufferSize < 1) return;

  struct winsize winSize;
  memset(&winSize, 0, sizeof(struct winsize));
  ioctl(STDIN_FILENO, TIOCGWINSZ, &winSize);

  if(winSize.ws_row < 2) return;
  
  if(inputBufferSize > winSize.ws_col) {
    // printf("\033[H\033[%iB\033[0K", winSize.ws_row-2);
    // printf("%.*s", winSize.ws_col, inputBufer+(inputBufferSize-winSize.ws_col));
    printf("\033[H\033[%iB\033[0K%.*s", winSize.ws_row-2, winSize.ws_col, inputBufer+(inputBufferSize-winSize.ws_col));
  } else {
    // printf("\033[H\033[%iB\033[0K", winSize.ws_row-2);
    // printf("%.*s", inputBufferSize, inputBufer);
    printf("\033[H\033[%iB\033[0K%.*s", winSize.ws_row-2, inputBufferSize, inputBufer);
  }
  fflush(stdout);
}
