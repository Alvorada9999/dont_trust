#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "common.h"
#include "init.h"
#include "dt_signal_util.h"
#include "terminal.h"

AllMessages allMessages;
void sigWinchHandler(int sigNumber) {
  renderMessages(&allMessages);
}

int main(int argc, char *argv[]) {
  struct sigaction newSigAction;

  memset(&newSigAction, 0, sizeof(struct sigaction));
  newSigAction.sa_handler = &sigWinchHandler;
  sigaction(SIGWINCH, &newSigAction, NULL);

  //test ----------------

  Message A, B, C;
  A.string = "The brain is XXXXXXXXXXXXXXXXXXXXXXX really little more than a collection of electrical signals. If we can learn to catalogue those then, in theory, you could upload someone's mind into a computer, allowing them to live forever as a digital form of consciousness, just like in the Johnny Depp film Transcendence.";
  A.size = 312;
  A.status = PEER_NOT_READ;
  A.nextMessage = &B;
  B.string = "But it's not just science fiction. Sure, scientists aren't anywhere near close to achieving such a feat with humans (and even if they could, the ethics would be pretty fraught), but there's few better examples than the time an international team of researchers managed to do just that with the roundworm Caenorhabditis elegans.";
  B.size = 327;
  B.status = RECEIVED;
  B.nextMessage = &C;
  C.string = "C. elegans is a little nematodes that have been extensively studied by scientists - we know all their genes and their nervous system has been analysed many times.";
  C.size = 162;
  C.status = PEER_READ;
  C.nextMessage = NULL;

  allMessages.sizeInChars = 351;
  allMessages.messagesInOrder[0] = &A;
  allMessages.messagesInOrder[1] = &B;
  allMessages.messagesInOrder[2] = &C;
  allMessages.currentStartingMessage = &A;
  allMessages.currentStartingMessageCharPosition = 1;
  allMessages.numberOfMessages = 3;

  //test ----------------

  Configs configs;
  memset(&configs, 0, sizeof(Configs));
  configs.shouldActAsServer = true;
  if(blockAllSignals() != 0)
    return -1;
  //to do - thread creation
  if(unblockIntAndWinchSignals() != 0)
    return -2;
  getConfigs(argc, argv, &configs);

  struct termios oldTerminalConfigurations;
  memset(&oldTerminalConfigurations, 0, sizeof(struct termios));
  if(setCbreak(STDIN_FILENO, &oldTerminalConfigurations) != 0)
    return -3;

  char inputBuffer[65535];
  unsigned int inputBufferSize = 0;
  int lastReadSize;
  char processingBuffer[65535];

  char *delSequence = "\x1b[1D\x1b[0K";
  while(true) {
    lastReadSize = read(STDIN_FILENO, processingBuffer, 65535);
    for(int i=0; i<lastReadSize; i++) {
      switch (processingBuffer[i]) {
        case NULLBYTE:
          break;
        case DEL:
          write(STDOUT_FILENO, delSequence, sizeof(strlen(delSequence)));
          if(inputBufferSize > 0) {
            inputBufferSize -= 1;
          }
          break;
        case LINEFEED:
          write(STDOUT_FILENO, "\n", 1);
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
