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

#include <stdbool.h>

#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

#include "common.h"
#include "terminal.h"

void processInput(AllMessages *allMessages) {
  static char inputBuffer[MAX_MESSAGE_SIZE];
  static uint16_t inputBufferSize = 0;
  static int16_t lastReadSize = 0;
  static char processingBuffer[MAX_MESSAGE_SIZE];
  static uint8_t applicationMode = EDIT;

  lastReadSize = read(STDIN_FILENO, processingBuffer, MAX_MESSAGE_SIZE);
  for(int16_t i=0; i<lastReadSize; i++) {
    switch (applicationMode) {
      case EDIT:
        switch (processingBuffer[i]) {
          case STATUS_CHANGE_BUTTON:
            applicationMode = VIEW;
            break;
          case DEL:
            if(inputBufferSize > 0) {
              inputBufferSize -= 1;
              renderCurrentlyBeingWrittenMessage(inputBuffer, inputBufferSize, &allMessages->winSize);
            }
            break;
          case LINEFEED:
            if(inputBufferSize > 0) {
              addNewMessage(allMessages, inputBuffer, inputBufferSize, MESSAGE_FROM_MYSELF);
              if(allMessages->isThereSpaceLeftOnScreenForMoreMessages) {
                raise(SIGWINCH);
              }
              inputBufferSize = 0;
              renderCurrentlyBeingWrittenMessage(inputBuffer, inputBufferSize, &allMessages->winSize);
            }
            break;
          default:
            //input validation, refer to ascii table
            if(inputBufferSize < MAX_MESSAGE_SIZE && *(processingBuffer+i) > 31 && *(processingBuffer+i) < 127) {
              inputBuffer[inputBufferSize] = processingBuffer[i];
              inputBufferSize += 1;
              renderCurrentlyBeingWrittenMessage(inputBuffer, inputBufferSize, &allMessages->winSize);
            }
        }
        break;
      case VIEW:
        switch (processingBuffer[i]) {
          case STATUS_CHANGE_BUTTON:
            applicationMode = EDIT;
            break;
          case DOWN_BUTTON:
            updatePostion(allMessages, 'j');
            raise(SIGWINCH);
            break;
          case UP_BUTTON:
            updatePostion(allMessages, 'k');
            raise(SIGWINCH);
            break;
        }
        break;
    }
  }
}
