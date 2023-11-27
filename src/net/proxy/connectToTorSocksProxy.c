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

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "error.h"
#include "net.h"

int8_t connectToTorSocksProxy(char *onionAddr, uint16_t portNumber) {
  if (strlen(onionAddr) != 62) errExit(5);
  char connectionRequest[7 + 62];

  // adding socks version
  connectionRequest[0] = 5;

  // socks command
  connectionRequest[1] = 1;

  // socks Rsv
  connectionRequest[2] = 0;

  // socks dst addr
  // domain name addr type
  connectionRequest[3] = 3;
  // domain name length equalls onion v3 addr size
  connectionRequest[4] = 62;
  memcpy(connectionRequest + 5, onionAddr, 62);

  // socks dst port
  uint16_t socksDstPort = htons(portNumber);
  memcpy(connectionRequest + 67, &socksDstPort, 2);

  struct sockaddr_in proxyAddr;
  memset(&proxyAddr, 0, sizeof(struct sockaddr_in));
  proxyAddr.sin_family = AF_INET;
  proxyAddr.sin_port = htons(DEFAULT_TOR_PROXY_PORT);
  proxyAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  int8_t socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if(socketFd == -1) errExit(18);
  if (connect(socketFd, (struct sockaddr *)&proxyAddr, sizeof(struct sockaddr_in)) == 0) {
    ssize_t writtenSize = 0;
    ssize_t readSize = 0;
    char response[6];
    memset(response, 0, 6);

    char gretting[3] = {5, 1, 0};
    while (writtenSize>-1 && writtenSize < 3) {
      writtenSize += write(socketFd, gretting + writtenSize, 3-writtenSize);
    }

    while (readSize < 2) {
      readSize += read(socketFd, response + readSize, 2);
    }
    if (response[1] != 0) errExit(6);

    writtenSize = 0;
    readSize = 0;

    while (writtenSize>-1 && writtenSize < 7 + 62) {
      writtenSize += write(socketFd, connectionRequest + writtenSize, (7 + 62)-writtenSize);
    }

    memset(response, 0, 6);
    while (readSize < 6) {
      readSize += read(socketFd, response + readSize, 6);
    }

    if (response[1] != 0x00) {
      switch (response[1]) {
      case 0x01:
        errExit(7);
        break;
      case 0x02:
        errExit(8);
        break;
      case 0x03:
        errExit(9);
        break;
      case 0x04:
        errExit(10);
        break;
      case 0x05:
        errExit(11);
        break;
      case 0x06:
        errExit(12);
        break;
      case 0x07:
        errExit(13);
        break;
      case 0x08:
        errExit(14);
        break;
      }
    }
  } else {
    close(socketFd);
    errExit(15);
  }

  return socketFd;
}
