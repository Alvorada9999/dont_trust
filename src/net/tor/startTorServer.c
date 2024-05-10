// This file is part of donttrust.
// Copyright (C) 2024 Kenedy Henrique Bueno Silva

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

#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "tor.h"
#include "error.h"
#include "dt_string_util.h"

#include <stdio.h>

int8_t startTorServer(char *password, int32_t passwordLength, int8_t *torSocketFd) {
  const char protocolInfoCOMMAND[] = { 'P','R','O','T','O','C','O','L','I','N','F','O',' ','1','\r','\n' };
  char authenticateCOMMAND[2019] = { 'A','U','T','H','E','N','T','I','C','A','T','E',' ','"', };
  char addOnionCOMMAND[45] = { 'A','D','D','_','O','N','I','O','N',' ','N','E','W',':','B','E','S','T',' ','P','o','r','t','=','8','0',',','1','2','7','.','0','.','0','.','1',':','8','0','0','0','\r','\n','\r','\n' };

  struct sockaddr_in torAddr;
  memset(&torAddr, 0, sizeof(struct sockaddr_in));
  torAddr.sin_family = AF_INET;

  torAddr.sin_family = AF_INET;
  torAddr.sin_port = htons(9051);
  torAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  *torSocketFd = socket(AF_INET, SOCK_STREAM, 0);

  if(connect(*torSocketFd, (struct sockaddr *)&torAddr, sizeof(struct sockaddr_in)) == -1) {
    errExit(38);
  };

  static char buffer[65536];
  static int32_t linesPositionsIndexs[10];
  static int32_t totalReadSize;
  static char status[3];

  sendCommand(*torSocketFd, (char*)protocolInfoCOMMAND, 16);
  totalReadSize = getReply(*torSocketFd, buffer, status);
  getLinePositions(buffer, totalReadSize, linesPositionsIndexs, 10);
  if(getIndex(buffer+linesPositionsIndexs[1], linesPositionsIndexs[2], " 1", 2) == -1) {
    errExit(41);
  }
  if(!isStatusEquallsTo(status, "250")) {
    errExit(39);
  }

  int32_t scapedPasswordSize = scapePassword(password, passwordLength, (char*)authenticateCOMMAND+14);
  authenticateCOMMAND[14+scapedPasswordSize] = '"';
  authenticateCOMMAND[14+scapedPasswordSize+1] = '\r';
  authenticateCOMMAND[14+scapedPasswordSize+2] = '\n';

  sendCommand(*torSocketFd, (char*)authenticateCOMMAND, 14+scapedPasswordSize+3);
  totalReadSize = getReply(*torSocketFd, buffer, status);
  if(!isStatusEquallsTo(status, "250")) {
    errExit(40);
  }

  //server here
  int8_t listeningSocketFd = socket(AF_INET, SOCK_STREAM, 0);
  if(listeningSocketFd == -1) errExit(18);

  struct sockaddr_in localServerAddr;
  memset(&localServerAddr, 0, sizeof(struct sockaddr_in));
  localServerAddr.sin_family = AF_INET;
  localServerAddr.sin_port = htons(8000);
  localServerAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if(bind(listeningSocketFd, (struct sockaddr *)&localServerAddr, sizeof(struct sockaddr_in)) == -1) errExit(19);
  //server here

  sendCommand(*torSocketFd, (char*)addOnionCOMMAND, 45);
  totalReadSize = getReply(*torSocketFd, buffer, status);
  if(!isStatusEquallsTo(status, "250")) {
    errExit(42);
  }

  printf("Give the following to your peer:\n%.*s.onion\n\nWaiting connection\n", 56, buffer+getIndex(buffer, totalReadSize, "ServiceID=", 10)+10);
  fflush(stdout);

  if(listen(listeningSocketFd, 1) == -1) errExit(20);
  int8_t connectedSocketFd = accept(listeningSocketFd, NULL, NULL);
  if(connectedSocketFd == -1) errExit(21);

  return connectedSocketFd;
}
