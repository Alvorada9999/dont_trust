// This file is part of dont_trust.
// Copyright (C) 2024 Kenedy Henrique Bueno Silva

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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "common.h"
#include "init.h"
#include "terminal.h"
#include "net.h"
#include "tor.h"

AllMessages allMessages;
MessageCodesToBeSentBackQueue messageCodesToBeSentBackAsConfirmationQueue = { .firstElement = NULL, .lastElement = NULL, .size = 0 };

void sigWinchHandler(int sigNumber) {
  ioctl(STDIN_FILENO, TIOCGWINSZ, &allMessages.winSize);
  clearMessages(&allMessages.winSize);
  renderMessages(&allMessages);
}

void handleSocketIo(int signalNumber, siginfo_t *info, void *x) {
  static int32_t event;
  //if the signal was received from the same process that
  //sent it, a write operation should be performed
  if(info->si_code == SI_QUEUE) {
    event = POLL_OUT;
  } else {
    event = info->si_code;
  }

  switch (event) {
    case POLL_IN: {
      allMessages.socketInputStatus.isInputAvailable = true;
      break;
    }
    case POLL_OUT: {
      allMessages.socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = true;
      break;
    }
  }
}

void sigIntHandler(int signalNumber) {
  exit(1);
}

struct termios oldTerminalConfigurations;
void resetTerminal(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalConfigurations);
  printf("\033[0m\033[H\033[0J\033[3J");
  fflush(stdout);
}

int8_t peerConnectedSocket = 0;
int8_t torSocketFd = 0;
void closeConnection(void) {
  shutdown(peerConnectedSocket, SHUT_RDWR);
  shutdown(torSocketFd, SHUT_RDWR);
}

int main(int argc, char *argv[]) {
  struct sigaction newSigAction;
  memset(&newSigAction, 0, sizeof(struct sigaction));
  newSigAction.sa_handler = &sigWinchHandler;
  sigaction(SIGWINCH, &newSigAction, NULL);
  newSigAction.sa_handler = &sigIntHandler;
  sigaction(SIGINT, &newSigAction, NULL);

  atexit(&resetTerminal);
  atexit(&closeConnection);

  setDefaultValues(&allMessages);
  updateBackground(&allMessages.winSize);
  welcomingMessage();
  memset(&oldTerminalConfigurations, 0, sizeof(struct termios));
  setCbreak(STDIN_FILENO, &oldTerminalConfigurations);

  Configs configs;
  memset(&configs, 0, sizeof(Configs));
  configs.shouldActAsServer = true;
  getConfigs(argc, argv, &configs);

  if(configs.shouldActAsServer) {
    if(configs.torControlPassword) {
      peerConnectedSocket = startTorServer(configs.torControlPassword, configs.torControlPasswordLength, &torSocketFd);
    } else {
      printNow("Waiting connection\n");
      peerConnectedSocket = startServer();
    }
  } else {
    printNow("Establishing connection\n");
    switch (configs.chosenOption) {
      case IPV4_ADDR:
        peerConnectedSocket = simpleConnect(configs.ipV4);
        break;
      case ONION_ADDR:
        peerConnectedSocket = connectToTorSocksProxy(configs.onionAddress, DEFAULT_TOR_SERVER_PORT);
        break;
    }
  }
  printNow("Connection established\n");

  enableSignalDrivenIoOnSocket(peerConnectedSocket, &handleSocketIo);

  while(true) {
    if(allMessages.socketInputStatus.isInputAvailable) {
      readFromPeer(&allMessages, &messageCodesToBeSentBackAsConfirmationQueue, &peerConnectedSocket, configs.pKey, &configs);
    }
    if(allMessages.socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer) {
      writeToPeer(&allMessages, &messageCodesToBeSentBackAsConfirmationQueue, &peerConnectedSocket, configs.pubKey);
    }
    processInput(&allMessages);
  }
}
