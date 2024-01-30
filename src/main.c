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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "common.h"
#include "init.h"
#include "terminal.h"
#include "net.h"

AllMessages allMessages;
MessageCodesToBeSentBackQueue messageCodesToBeSentBackAsConfirmationQueue = { .firstElement = NULL, .lastElement = NULL, .size = 0 };

void sigWinchHandler(int sigNumber) {
  clearTerminal();
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

struct termios oldTerminalConfigurations;
void resetTerminal(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalConfigurations);
}

int main(int argc, char *argv[]) {
  Configs configs;
  memset(&configs, 0, sizeof(Configs));
  configs.shouldActAsServer = true;

  getConfigs(argc, argv, &configs);

  int8_t peerConnectedSocket = 0;
  if(configs.shouldActAsServer) {
    peerConnectedSocket = startServer();
  } else {
    switch (configs.chosenOption) {
      case IPV4_ADDR:
        peerConnectedSocket = simpleConnect(configs.ipV4);
        break;
      case ONION_ADDR:
        peerConnectedSocket = connectToTorSocksProxy(configs.onionAddress, DEFAULT_SERVER_PORT);
        break;
    }
  }

  struct sigaction newSigAction;
  memset(&newSigAction, 0, sizeof(struct sigaction));
  newSigAction.sa_handler = &sigWinchHandler;
  sigaction(SIGWINCH, &newSigAction, NULL);

  enableSignalDrivenIoOnSocket(peerConnectedSocket, &handleSocketIo);

  //default values
  allMessages.sizeInChars = 0;
  allMessages.currentStartingMessage = NULL;
  allMessages.currentStartingMessageCharPosition = 0;
  allMessages.lastMessage = NULL;
  allMessages.isThereSpaceLeftOnScreenForMoreMessages = true;

  allMessages.messagesByCode.array = malloc(sizeof(Message)*DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY);
  allMessages.messagesByCode.availableSpace = DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY;
  allMessages.messagesByCode.currentSize = DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY;
  allMessages.messagesByCode.length = 0;
  allMessages.messagesByCode.numberOfSentMessages = 0;

  allMessages.socketOutputStatus.isThereAnythingBeingSent = false;
  allMessages.socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = true;
  allMessages.socketInputStatus.isInputAvailable = false;

  memset(&oldTerminalConfigurations, 0, sizeof(struct termios));
  setCbreak(STDIN_FILENO, &oldTerminalConfigurations);
  atexit(&resetTerminal);

  while(true) {
    if(allMessages.socketInputStatus.isInputAvailable) {
      readFromPeer(&allMessages, &messageCodesToBeSentBackAsConfirmationQueue, &peerConnectedSocket);
    }
    if(allMessages.socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer) {
      writeToPeer(&allMessages, &messageCodesToBeSentBackAsConfirmationQueue, &peerConnectedSocket);
    }
    processInput(&allMessages);
  }
}
