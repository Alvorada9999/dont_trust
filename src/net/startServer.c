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

#include <sys/socket.h>
#include <arpa/inet.h>

#include "net.h"
#include "error.h"

int8_t startServer(void) {
  int8_t listeningSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if(listeningSocketFd == -1) errExit(18);

  struct sockaddr_in localServerAddr;
  memset(&localServerAddr, 0, sizeof(struct sockaddr_in));
  localServerAddr.sin_family = AF_INET;
  localServerAddr.sin_port = htons(DEFAULT_SERVER_PORT);
  localServerAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if(bind(listeningSocketFd, (struct sockaddr *)&localServerAddr, sizeof(struct sockaddr_in)) == -1) errExit(19);

  if(listen(listeningSocketFd, 1) == -1) errExit(20);

  int8_t connectedSocketFd = accept(listeningSocketFd, NULL, NULL);
  if(connectedSocketFd == -1) errExit(21);

  return connectedSocketFd;
}
