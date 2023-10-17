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

int renderMessages(AllMessages *allMessages) {
  //clear terminal
  write(STDOUT_FILENO, "\033[H\033[0J\033[3J", sizeof("\033[H\033[0J\033[3J"));

  //stop if there are no messages
  if(allMessages->startingMessage == NULL) return 0;
  struct winsize winSize;
  memset(&winSize, 0, sizeof(struct winsize));
  ioctl(STDIN_FILENO, TIOCGWINSZ, &winSize);
  // the last two rows are used to show some info and the text that is currently being typed
  u_int32_t amountOfCharsThatCanBeShow = (winSize.ws_row - 2) * winSize.ws_col;
  if(winSize.ws_row < 3) return 0;
  char *textToOutput = malloc(amountOfCharsThatCanBeShow);
  memset(textToOutput, 0, amountOfCharsThatCanBeShow);
  u_int32_t textToOutputWrittenSize = 0;

  u_int16_t remainingRowSpace = winSize.ws_col, currentMessageReadSizeInBytes = 0;
  u_int16_t wordSize = 0;
  Message *currentMessage = allMessages->startingMessage;
  do {
    while(currentMessageReadSizeInBytes < currentMessage->size && textToOutputWrittenSize < amountOfCharsThatCanBeShow) {
      wordSize = getWordSize(currentMessage->string+currentMessageReadSizeInBytes, currentMessage->size - currentMessageReadSizeInBytes);
      if(wordSize <= winSize.ws_col) {
        if(wordSize <= remainingRowSpace) {
          for (u_int16_t i=0; i<wordSize && textToOutputWrittenSize < amountOfCharsThatCanBeShow; i++) {
            textToOutput[textToOutputWrittenSize] = currentMessage->string[currentMessageReadSizeInBytes];

            textToOutputWrittenSize += 1;
            currentMessageReadSizeInBytes += 1;
            remainingRowSpace -= 1;
          }
        } else {
          for (u_int16_t i=0; i<remainingRowSpace && textToOutputWrittenSize < amountOfCharsThatCanBeShow; i++) {
            textToOutput[textToOutputWrittenSize] = ESPACE;

            textToOutputWrittenSize += 1;
          }
          remainingRowSpace = winSize.ws_col;

          for (u_int16_t i=0; i<wordSize && textToOutputWrittenSize < amountOfCharsThatCanBeShow; i++) {
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

        for (u_int16_t i=0; i<wordSize && textToOutputWrittenSize < amountOfCharsThatCanBeShow; i++) {
          textToOutput[textToOutputWrittenSize] = currentMessage->string[currentMessageReadSizeInBytes];

          textToOutputWrittenSize += 1;
          currentMessageReadSizeInBytes += 1;
        }
      }
    }

    currentMessage = currentMessage->nextMessage;
    currentMessageReadSizeInBytes = 0;
  } while(currentMessage != NULL && textToOutputWrittenSize < amountOfCharsThatCanBeShow);

  ssize_t writtenSize = 0;
  while(writtenSize < amountOfCharsThatCanBeShow) {
    writtenSize += write(STDOUT_FILENO, textToOutput+writtenSize, amountOfCharsThatCanBeShow);
  }

  free(textToOutput);
  return 0;
}
