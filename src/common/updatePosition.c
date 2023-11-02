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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/ioctl.h>

#include "common.h"

int8_t updatePostion(AllMessages *allMessages, char jOrK) {
  static struct winsize winSize;
  memset(&winSize, 0, sizeof(struct winsize));
  ioctl(STDIN_FILENO, TIOCGWINSZ, &winSize);

  uint32_t remainingRowSpace = winSize.ws_col;
  uint16_t wordSize = 0;
  Message *currentMessage = allMessages->currentStartingMessage;
  uint32_t readSize;
  if(currentMessage == NULL) return 0;

  switch (jOrK) {
    case 'j':
      readSize = allMessages->currentStartingMessageCharPosition;

      if(allMessages->currentStartingMessageCharPosition == currentMessage->size && currentMessage->nextMessage != NULL) {
        allMessages->currentStartingMessage = currentMessage->nextMessage;
        allMessages->currentStartingMessageCharPosition = 0;
        break;
      }
      while (wordSize <= remainingRowSpace && readSize < currentMessage->size) {
        wordSize = getWordSize(currentMessage->string+readSize, currentMessage->size - readSize);
        if(wordSize <= remainingRowSpace) {
          remainingRowSpace -= wordSize;
          readSize += wordSize;
          wordSize = getWordSize(currentMessage->string+readSize, currentMessage->size - readSize);
        }
      }

      if(wordSize <= winSize.ws_col){
        allMessages->currentStartingMessageCharPosition = readSize;
      } else {
        allMessages->currentStartingMessageCharPosition = readSize + remainingRowSpace;
      }
      break;
    case 'k':
      readSize = 0;

      if(allMessages->currentStartingMessageCharPosition == 0) {
        if(currentMessage->previousMessage != NULL) {
          allMessages->currentStartingMessage = currentMessage->previousMessage;
          allMessages->currentStartingMessageCharPosition = currentMessage->previousMessage->size;
          return 0;
        }
        return 0;
      }

      uint32_t currentLineStartingPosition = 0, previousLineStartingPosition = 0;
      while (readSize < allMessages->currentStartingMessageCharPosition) {
        wordSize = getWordSize(currentMessage->string+readSize, currentMessage->size - readSize);
        if(wordSize <= remainingRowSpace) {
          remainingRowSpace -= wordSize;
          readSize += wordSize;
        } else {
          previousLineStartingPosition = currentLineStartingPosition;
          if(wordSize <= winSize.ws_col) {
            currentLineStartingPosition = readSize;
            readSize += wordSize;
            remainingRowSpace = winSize.ws_col - wordSize;
          } else {
            currentLineStartingPosition = readSize + remainingRowSpace;
            readSize += remainingRowSpace;
            remainingRowSpace = winSize.ws_col;
          }
        }
      }

      if(readSize == currentLineStartingPosition) {
        allMessages->currentStartingMessageCharPosition = previousLineStartingPosition;
        break;
      }
      allMessages->currentStartingMessageCharPosition = currentLineStartingPosition;

      break;
  }
  return 0;
}
