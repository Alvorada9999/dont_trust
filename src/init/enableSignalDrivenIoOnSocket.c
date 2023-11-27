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

#define _GNU_SOURCE

#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include "init.h"

void enableSignalDrivenIoOnSocket(int8_t socketFd, void (*handler)(int, siginfo_t *, void *)) {
  struct sigaction newSigAction;
  memset(&newSigAction, 0, sizeof(struct sigaction));
  newSigAction.sa_sigaction = handler;
  newSigAction.sa_flags = SA_SIGINFO;
  sigaction(SIGRTMIN, &newSigAction, NULL);

  fcntl(socketFd, F_SETOWN, getpid());

  int32_t fileStatusFlags = fcntl(socketFd, F_GETFL);
  fcntl(socketFd, F_SETFL, fileStatusFlags | O_ASYNC | O_NONBLOCK);

  fcntl(socketFd, F_SETSIG, SIGRTMIN);
}
