#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "common.h"
#include "terminal.h"

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

int8_t renderMessages(AllMessages *allMessages) {
  static char *cleartTerminalEscapeSequence = "\033[H\033[0J\033[3J";
  static u_int8_t clearTerminalEscapeSequenceSize = strlen("\033[H\033[0J\033[3J");

  //stop if there are no messages
  if(allMessages->startingMessage == NULL) return 0;
  struct winsize winSize;
  memset(&winSize, 0, sizeof(struct winsize));
  ioctl(STDIN_FILENO, TIOCGWINSZ, &winSize);
  // the last two rows are used to show some info and the text that is currently being typed
  u_int32_t amountOfCharsThatCanBeShow = (winSize.ws_row - 2) * winSize.ws_col;
  if(winSize.ws_row < 3) return 0;
  if(amountOfCharsThatCanBeShow > DEFAULT_MESSAGE_OUTPUT_SIZE) return -4;
  //let's compromise to use this many memory since malloc is not async-signal-safe
  static char textToOutput[DEFAULT_MESSAGE_OUTPUT_SIZE];
  memset(textToOutput, 0, DEFAULT_MESSAGE_OUTPUT_SIZE);
  u_int32_t textToOutputWrittenSize = clearTerminalEscapeSequenceSize;
  for (u_int32_t i=0; i<clearTerminalEscapeSequenceSize; i++) {
    textToOutput[i] = cleartTerminalEscapeSequence[i];
  }
  //should add the max amount of possible messages too to be able to handle
  //backgroung color changing escape sequences between each message
  u_int16_t maxWritingSize = amountOfCharsThatCanBeShow + clearTerminalEscapeSequenceSize;

  u_int16_t remainingRowSpace = winSize.ws_col, currentMessageReadSizeInBytes = 0;
  u_int16_t wordSize = 0;
  Message *currentMessage = allMessages->startingMessage;
  do {
    while(currentMessageReadSizeInBytes < currentMessage->size && textToOutputWrittenSize < maxWritingSize) {
      wordSize = getWordSize(currentMessage->string+currentMessageReadSizeInBytes, currentMessage->size - currentMessageReadSizeInBytes);
      if(wordSize <= winSize.ws_col) {
        if(wordSize <= remainingRowSpace) {
          for (u_int16_t i=0; i<wordSize && textToOutputWrittenSize < maxWritingSize; i++) {
            textToOutput[textToOutputWrittenSize] = currentMessage->string[currentMessageReadSizeInBytes];

            textToOutputWrittenSize += 1;
            currentMessageReadSizeInBytes += 1;
            remainingRowSpace -= 1;
          }
        } else {
          for (u_int16_t i=0; i<remainingRowSpace && textToOutputWrittenSize < maxWritingSize; i++) {
            textToOutput[textToOutputWrittenSize] = ESPACE;

            textToOutputWrittenSize += 1;
          }
          remainingRowSpace = winSize.ws_col;

          for (u_int16_t i=0; i<wordSize && textToOutputWrittenSize < maxWritingSize; i++) {
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

        for (u_int16_t i=0; i<wordSize && textToOutputWrittenSize < maxWritingSize; i++) {
          textToOutput[textToOutputWrittenSize] = currentMessage->string[currentMessageReadSizeInBytes];

          textToOutputWrittenSize += 1;
          currentMessageReadSizeInBytes += 1;
        }
      }
    }

    //before each message, a escape sequence is added
    //"textToOutputWrittenSize" must be increased to avoid writing above the escape sequence
    //"maxWritingSize" must also be increased to don't decrease the amount of chars that can be show

    currentMessage = currentMessage->nextMessage;
    currentMessageReadSizeInBytes = 0;
  } while(currentMessage != NULL && textToOutputWrittenSize < maxWritingSize);

  ssize_t writtenSize = 0;
  while(writtenSize < maxWritingSize) {
    writtenSize += write(STDOUT_FILENO, textToOutput+writtenSize, maxWritingSize);
  }

  return 0;
}
