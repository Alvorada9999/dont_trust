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

  Message A, B, C, D, E, F, G, H, I, J;
  A.string = "The brain is XXXXXXXXXXXXXXXXXXXXXXX really little more than a collection of @@@@@@@@@@@@@@@@@@@@@@@ electrical signals. If we can learn to catalogue those then, in theory, you could upload someone's mind into a computer, allowing them to live forever as a digital form of consciousness, just like in the Johnny Depp film Transcendence.";
  A.size = 336;
  A.status = PEER_NOT_READ;
  A.nextMessage = &B;
  A.previousMessage = NULL;
  B.string = "But it's not just science fiction. Sure, scientists aren't anywhere near close to achieving such a feat with humans (and even if they could, the ethics would be pretty fraught), but there's few better examples than the time an international team of researchers managed to do just that with the roundworm Caenorhabditis elegans.";
  B.size = 327;
  B.status = RECEIVED;
  B.nextMessage = &C;
  B.previousMessage = &A;
  C.string = "C. elegans is a little nematodes that have been extensively studied by scientists - we know all their genes and their nervous system has been analysed many times.";
  C.size = 162;
  C.status = PEER_READ;
  C.nextMessage = &D;
  C.previousMessage = &B;
  D.string = "So, in 2014, a collective called the OpenWorm project mapped all the connections between the worm's 302 neurons and managed to simulate them in software, as Marissa Fessenden reports for the Smithsonian.";
  D.size = 203;
  D.status = PEER_NOT_READ;
  D.nextMessage = &E;
  D.previousMessage = &C;
  E.string = "The ultimate goal of the project was to completely replicate C. elegans as a virtual organism. But as an amazing starting point, they managed to simulate its brain, and then they uploaded that into a simple Lego robot.";
  E.size = 218;
  E.status = RECEIVED;
  E.nextMessage = &F;
  E.previousMessage = &D;
  F.string = "This Lego robot has all the equivalent limited body parts that C. elegans has - a sonar sensor that acts as a nose, and motors that replace the worm's motor neurons on each side of its body.";
  F.size = 190;
  F.status = PEER_READ;
  F.nextMessage = &G;
  F.previousMessage = &E;

  G.string = "This Lego robot has all the equivalent limited body parts that C. elegans has - a sonar sensor that acts as a nose, and motors that replace the worm's motor neurons on each side of its body.";
  G.status = PEER_NOT_READ;
  G.size = 190;
  G.nextMessage = &H;
  G.previousMessage = &F;

  H.string = "This Lego robot has all the equivalent limited body parts that C. elegans has - a sonar sensor that acts as a nose, and motors that replace the worm's motor neurons on each side of its body.";
  H.size = 190;
  H.status = RECEIVED;
  H.nextMessage = &I;
  H.previousMessage = &G;

  I.string = "This Lego robot has all the equivalent limited body parts that C. elegans has - a sonar sensor that acts as a nose, and motors that replace the worm's motor neurons on each side of its body.";
  I.size = 190;
  I.status = PEER_READ;
  I.nextMessage = &J;
  I.previousMessage = &H;

  J.string = "This Lego robot has all the equivalent limited body parts that C. elegans has - a sonar sensor that acts as a nose, and motors that replace the worm's motor neurons on each side of its body.";
  J.size = 190;
  J.status = PEER_NOT_READ;
  J.nextMessage = NULL;
  J.previousMessage = &I;

  allMessages.sizeInChars = 2196;
  allMessages.messagesInOrder[0] = &A;
  allMessages.messagesInOrder[1] = &B;
  allMessages.messagesInOrder[2] = &C;
  allMessages.messagesInOrder[3] = &D;
  allMessages.messagesInOrder[4] = &E;
  allMessages.messagesInOrder[5] = &F;
  allMessages.messagesInOrder[6] = &G;
  allMessages.messagesInOrder[7] = &H;
  allMessages.messagesInOrder[8] = &I;
  allMessages.messagesInOrder[9] = &J;
  allMessages.currentStartingMessage = &A;
  allMessages.currentStartingMessageCharPosition = 0;

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
  uint8_t applicationMode = EDIT;
  while(true) {
    lastReadSize = read(STDIN_FILENO, processingBuffer, 65535);
    for(int i=0; i<lastReadSize; i++) {
      switch (applicationMode) {
        case EDIT:
          switch (processingBuffer[i]) {
            case ESCAPE:
              applicationMode = VIEW;
              break;
            case NULLBYTE:
              break;
            case DEL:
              write(STDOUT_FILENO, delSequence, sizeof(strlen(delSequence)));
              if(inputBufferSize > 0) {
                inputBufferSize -= 1;
              }
              break;
            default:
              //input validation, refer to ascii table
              if(*(processingBuffer+i) > 31 && *(processingBuffer+i) < 127) {
                write(STDOUT_FILENO, processingBuffer+i, 1);
                inputBuffer[inputBufferSize] = processingBuffer[i];
                inputBufferSize += 1;
              }
          }
          break;
        case VIEW:
          switch (processingBuffer[i]) {
            case ESCAPE:
              applicationMode = EDIT;
              break;
            //down
            case 'j':
              updatePostion(&allMessages, 'j');
              raise(SIGWINCH);
              break;
            //up
            case 'k':
              updatePostion(&allMessages, 'k');
              raise(SIGWINCH);
              break;
          }
          break;
      }
    }
  }

  resetTerminal(STDIN_FILENO, &oldTerminalConfigurations);

  return 0;
}
