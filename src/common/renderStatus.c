// This file is part of donttrust.
// Copyright (C) 2023 Kenedy Henrique Bueno Silva

// donttrust is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include <stdint.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/ioctl.h>

#include "common.h"

void renderStatus(uint8_t type, struct winsize *winSize) {
  static uint32_t numberOfMessagesSent = 0;
  static uint32_t numberOfMessagesConfirmationsReceived = 0;
  static uint32_t numberOfMessagesReceived = 0;

  switch (type) {
    case MESSAGE_SENT:
      numberOfMessagesSent++;
      break;
    case MESSAGE_CONFIRMATION_RECEIVED:
      numberOfMessagesConfirmationsReceived++;
      break;
    case MESSAGE_RECEIVED:
      numberOfMessagesReceived++;
      break;
  }

  printf("\033[%i;0H\033[48;5;%sm%d/%d | %d\033[0K\033[48;5;%sm", winSize->ws_row, STATUS_LINE_TERMINAL_COLOR_CODE, numberOfMessagesConfirmationsReceived, numberOfMessagesSent, numberOfMessagesReceived, TERMINAL_BACKGROUND_COLOR_ID);
  fflush(stdout);
}
