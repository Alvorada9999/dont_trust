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

#include <stdio.h>

#include <signal.h>

#include "dt_signal_util.h"

void showSiCodeInfo(int32_t siCode) {
  switch (siCode) {
    case POLL_OUT:
      printf("POLL_OUT\n");
      break;
    case POLL_IN:
      printf("POLL_IN\n");
      break;
    case ILL_ILLADR:
      printf("illegal addressing mode\n");
      break;
    case ILL_ILLTRP:
      printf("illegal trap\n");
      break;
    case SI_USER:
      printf("Signal sent by kill()\n");
      break;
    case SI_MESGQ:
      printf("Signal generated by arrival of a message on an empty message queue\n");
      break;
    case POLL_PRI:
      printf("High priority input available.\n");
      break;
    case POLL_HUP:
      printf("Device disconnected.\n");
      break;
    case SI_QUEUE:
      printf("Signal sent by sigqueue().\n");
      break;
    case SI_TIMER:
      printf("Signal generated by expiration of a timer set by timer_settime().\n");
      break;
    case SI_ASYNCIO:
      printf("Signal generated by completion of an asynchronous I/O request.\n");
      break;
    case FPE_FLTINV:
      printf("Invalid floating-point operation.\n");
      break;
    case FPE_FLTSUB:
      printf("Subscript out of range.\n");
      break;
  }
  fflush(stdout);
}
