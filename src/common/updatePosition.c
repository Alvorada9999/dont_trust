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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/ioctl.h>

#include "common.h"

int8_t updatePostion(ProgramData *programData, char jOrK) {
  uint32_t remainingRowSpace = programData->winSize.ws_col;
  uint16_t wordSize = 0;
  Message *currentMessage = programData->currentStartingMessage;
  uint32_t readSize;
  if(currentMessage == NULL) return 0;

  switch (jOrK) {
    case 'j':
      if(currentMessage->size-programData->currentStartingMessageCharPosition <= remainingRowSpace && currentMessage->nextMessage != NULL) {
        programData->currentStartingMessage = currentMessage->nextMessage;
        programData->currentStartingMessageCharPosition = 0;
        currentMessage = programData->currentStartingMessage;
        break;
      }

      readSize = programData->currentStartingMessageCharPosition;

      while (wordSize <= remainingRowSpace && readSize < currentMessage->size) {
        wordSize = getWordSize(currentMessage->string+readSize, currentMessage->size - readSize);
        if(wordSize <= remainingRowSpace) {
          remainingRowSpace -= wordSize;
          readSize += wordSize;
          wordSize = getWordSize(currentMessage->string+readSize, currentMessage->size - readSize);
        }
      }

      if(wordSize <= programData->winSize.ws_col){
        programData->currentStartingMessageCharPosition = readSize;
      } else {
        programData->currentStartingMessageCharPosition = readSize + remainingRowSpace;
      }
      break;
    case 'k':
      readSize = 0;

      if(programData->currentStartingMessageCharPosition == 0 && currentMessage->previousMessage != NULL) {
        programData->currentStartingMessage = currentMessage->previousMessage;
        programData->currentStartingMessageCharPosition = currentMessage->previousMessage->size;
        currentMessage = programData->currentStartingMessage;
      }

      uint32_t currentLineStartingPosition = 0, previousLineStartingPosition = 0;
      while (readSize < programData->currentStartingMessageCharPosition) {
        wordSize = getWordSize(currentMessage->string+readSize, currentMessage->size - readSize);
        if(wordSize <= remainingRowSpace) {
          remainingRowSpace -= wordSize;
          readSize += wordSize;
        } else {
          previousLineStartingPosition = currentLineStartingPosition;
          if(wordSize <= programData->winSize.ws_col) {
            currentLineStartingPosition = readSize;
            readSize += wordSize;
            remainingRowSpace = programData->winSize.ws_col - wordSize;
          } else {
            currentLineStartingPosition = readSize + remainingRowSpace;
            readSize += remainingRowSpace;
            remainingRowSpace = programData->winSize.ws_col;
          }
        }
      }

      if(readSize == currentLineStartingPosition) {
        programData->currentStartingMessageCharPosition = previousLineStartingPosition;
        break;
      }
      programData->currentStartingMessageCharPosition = currentLineStartingPosition;

      break;
  }
  return 0;
}
