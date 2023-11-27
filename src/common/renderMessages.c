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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "common.h"
#include "terminal.h"
#include "error.h"

u_int16_t getWordSize(char *from, u_int16_t stopAfterNBytes) {
  u_int16_t size = 0;
  if(from[0] == ' ') {
    for (u_int16_t i=0; i<stopAfterNBytes; i++) {
      if (from[i] == ' ') {
        size += 1;
      } else {
        break;
      }
    }
  } else {
    for (u_int16_t i=0; i<stopAfterNBytes; i++) {
      if (from[i] != ' ') {
        size += 1;
      } else {
        break;
      }
    }
  }
  return size;
}

void updateBackgroundColor(uint8_t status, char *textToOutput, uint32_t *textToOutputWrittenSize, uint32_t *maxWritingSize) {
  static char *redBackgroundEscapeSequence = "\033[41m";
  uint8_t redBackgroundEscapeSequenceSize = strlen("\033[41m");
  static char *greenBackgroundEscapeSequence = "\033[42m";
  uint8_t greenBackgroundEscapeSequenceSize = strlen("\033[42m");
  static char *resetBackgroundEscapeSequence = "\033[0m";
  uint8_t resetBackgroundEscapeSequenceSize = strlen("\033[0m");
  switch (status) {
    case PEER_READ:
      for (uint8_t i=0; i<greenBackgroundEscapeSequenceSize && *textToOutputWrittenSize < *maxWritingSize && *maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
        textToOutput[*textToOutputWrittenSize] = greenBackgroundEscapeSequence[i];
        (*textToOutputWrittenSize)++;
        (*maxWritingSize)++;
      }
      break;
    case PEER_NOT_READ:
      for (uint8_t i=0; i<redBackgroundEscapeSequenceSize && *textToOutputWrittenSize < *maxWritingSize && *maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
        textToOutput[*textToOutputWrittenSize] = redBackgroundEscapeSequence[i];
        (*textToOutputWrittenSize)++;
        (*maxWritingSize)++;
      }
      break;
    case RECEIVED:
      for (uint8_t i=0; i<resetBackgroundEscapeSequenceSize && *textToOutputWrittenSize < *maxWritingSize && *maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
        textToOutput[*textToOutputWrittenSize] = resetBackgroundEscapeSequence[i];
        (*textToOutputWrittenSize)++;
        (*maxWritingSize)++;
      }
      break;
    default:
      break;
  }
}

/* Add reset escape sequences 
to "textToOutput"*/
void addResetBackgroundColorEscapeSequence(char *textToOutput, uint32_t *textToOutputWrittenSize, uint32_t *maxWritingSize) {
  static char *resetBackgroundEscapeSequence = "\033[0m";
  uint8_t resetBackgroundEscapeSequenceSize = strlen("\033[0m");
  for (uint8_t i=0; i<resetBackgroundEscapeSequenceSize && *textToOutputWrittenSize < *maxWritingSize && *maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
    textToOutput[*textToOutputWrittenSize] = resetBackgroundEscapeSequence[i];
    (*textToOutputWrittenSize)++;
    (*maxWritingSize)++;
  }
}

int8_t renderMessages(AllMessages *allMessages) {
  //stop if there are no messages
  if(allMessages->currentStartingMessage == NULL) return 0;
  struct winsize winSize;
  memset(&winSize, 0, sizeof(struct winsize));
  ioctl(STDIN_FILENO, TIOCGWINSZ, &winSize);
  // the last two rows are used to show some info and the text that is currently being typed
  // the value at the right side of the equation won't be negative so no problem between different signedness, don't bother :)
  u_int32_t amountOfCharsThatCanBeShow = (winSize.ws_row - 2) * winSize.ws_col;
  if(winSize.ws_row < 3) return 0;
  if(amountOfCharsThatCanBeShow > DEFAULT_MESSAGE_OUTPUT_SIZE) errExit(4);
  //let's compromise to use this many memory since malloc is not async-signal-safe
  static char textToOutput[DEFAULT_MESSAGE_OUTPUT_SIZE];
  memset(textToOutput, 0, DEFAULT_MESSAGE_OUTPUT_SIZE);
  u_int32_t textToOutputWrittenSize = 0;
  u_int32_t maxWritingSize = amountOfCharsThatCanBeShow;

  u_int16_t remainingRowSpace = winSize.ws_col, currentMessageReadSizeInBytes = allMessages->currentStartingMessageCharPosition;
  u_int16_t wordSize = 0;
  Message *currentMessage = allMessages->currentStartingMessage;
  updateBackgroundColor(currentMessage->status, textToOutput, &textToOutputWrittenSize, &maxWritingSize);
  do {
    while(currentMessageReadSizeInBytes < currentMessage->size && textToOutputWrittenSize < maxWritingSize) {
      wordSize = getWordSize(currentMessage->string+currentMessageReadSizeInBytes, currentMessage->size - currentMessageReadSizeInBytes);
      if(wordSize <= winSize.ws_col) {
        if(wordSize <= remainingRowSpace) {
          for (u_int16_t i=0; i<wordSize && textToOutputWrittenSize < maxWritingSize && maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
            textToOutput[textToOutputWrittenSize] = currentMessage->string[currentMessageReadSizeInBytes];

            textToOutputWrittenSize += 1;
            currentMessageReadSizeInBytes += 1;
            remainingRowSpace -= 1;
          }
        } else {
          for (u_int16_t i=0; i<remainingRowSpace && textToOutputWrittenSize < maxWritingSize && maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
            textToOutput[textToOutputWrittenSize] = ESPACE;

            textToOutputWrittenSize += 1;
          }
          remainingRowSpace = winSize.ws_col;

          for (u_int16_t i=0; i<wordSize && textToOutputWrittenSize < maxWritingSize && maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
            textToOutput[textToOutputWrittenSize] = currentMessage->string[currentMessageReadSizeInBytes];
            
            textToOutputWrittenSize += 1;
            currentMessageReadSizeInBytes += 1;
            remainingRowSpace -= 1;
          }
        }
      } else {
        u_int16_t tempWordSize = wordSize;
        tempWordSize -= remainingRowSpace;
        while(tempWordSize > winSize.ws_col) {
          tempWordSize -= winSize.ws_col;
        }
        remainingRowSpace = winSize.ws_col - tempWordSize;

        for (u_int16_t i=0; i<wordSize && textToOutputWrittenSize < maxWritingSize && maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
          textToOutput[textToOutputWrittenSize] = currentMessage->string[currentMessageReadSizeInBytes];

          textToOutputWrittenSize += 1;
          currentMessageReadSizeInBytes += 1;
        }
      }
    }

    //to "break" a line
    for (u_int16_t i=0; i<remainingRowSpace && textToOutputWrittenSize < maxWritingSize && maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
      textToOutput[textToOutputWrittenSize] = ESPACE;
      textToOutputWrittenSize += 1;
    }
    //before "breaking" a line, reset background color
    addResetBackgroundColorEscapeSequence(textToOutput, &textToOutputWrittenSize, &maxWritingSize);
    for (u_int16_t i=0; i<winSize.ws_col && textToOutputWrittenSize < maxWritingSize && maxWritingSize < DEFAULT_MESSAGE_OUTPUT_SIZE; i++) {
      textToOutput[textToOutputWrittenSize] = ESPACE;
      textToOutputWrittenSize += 1;
    }
    remainingRowSpace = winSize.ws_col;


    currentMessage = currentMessage->nextMessage;
    currentMessageReadSizeInBytes = 0;

    //before each message, background color escape sequence is added
    //"textToOutputWrittenSize" must be increased to avoid writing above the escape sequence
    if(currentMessage != NULL && textToOutputWrittenSize < maxWritingSize) {
      updateBackgroundColor(currentMessage->status, textToOutput, &textToOutputWrittenSize, &maxWritingSize);
    }

  } while(currentMessage != NULL && textToOutputWrittenSize < maxWritingSize);
  addResetBackgroundColorEscapeSequence(textToOutput, &textToOutputWrittenSize, &maxWritingSize);

  ssize_t writtenSize = 0;
  while(writtenSize > -1 && writtenSize < maxWritingSize) {
    writtenSize += write(STDOUT_FILENO, textToOutput+writtenSize, maxWritingSize-writtenSize);
  }

  return 0;
}
