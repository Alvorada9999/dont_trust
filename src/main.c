#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>

#include "init.h"
#include "dt_signal_util.h"
#include "terminal.h"

int main(int argc, char *argv[]) {
  Configs configs;
  memset(&configs, 0, sizeof(Configs));
  configs.shouldActAsServer = true;
  if(blockAllSignals() != 0)
    return -1;
  if(unblockSigInt() != 0)
    return -2;
  getConfigs(argc, argv, &configs);

  struct termios oldTerminalConfigurations;
  memset(&oldTerminalConfigurations, 0, sizeof(struct termios));
  if(setCbreak(STDIN_FILENO, &oldTerminalConfigurations) != 0)
    return -3;

  char inputBuffer[64000];
  unsigned int inputBufferSize = 0;
  int lastReadSize;
  char processingBuffer[64000];

  char *sequence = "\x1b[1D\x1b[0K";
  while(true) {
    lastReadSize = read(STDIN_FILENO, processingBuffer, 64000);
    for(int i=0; i<lastReadSize; i++) {
      switch (processingBuffer[i]) {
        case DEL:
          write(STDOUT_FILENO, sequence, sizeof(strlen(sequence)));
          if(inputBufferSize > 0) {
            inputBufferSize -= 1;
          }
          break;
        case LINEFEED:
          write(STDOUT_FILENO, "\n", 1);
          write(STDOUT_FILENO, inputBuffer, inputBufferSize);
          break;
        default:
          write(STDOUT_FILENO, processingBuffer+i, 1);
          inputBuffer[inputBufferSize] = processingBuffer[i];
          inputBufferSize += 1;
      }
    }
  }

  resetTerminal(STDIN_FILENO, &oldTerminalConfigurations);

  return 0;
}
