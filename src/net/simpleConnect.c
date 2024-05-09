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

#include <arpa/inet.h>
#include <netinet/in.h>

#include <string.h>
#include <sys/socket.h>

#include "net.h"
#include "error.h"

int8_t simpleConnect(char *addr) {
  struct sockaddr_in peerAddr;
  memset(&peerAddr, 0, sizeof(struct sockaddr_in));
  peerAddr.sin_family = AF_INET;
  peerAddr.sin_port = htons(DEFAULT_SERVER_PORT);
  inet_pton(AF_INET, addr, &peerAddr.sin_addr);

  int8_t socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if(socketFd == -1) errExit(18);

  if(connect(socketFd, (struct sockaddr *)&peerAddr, sizeof(struct sockaddr_in)) == -1) errExit(22);

  return socketFd;
}
