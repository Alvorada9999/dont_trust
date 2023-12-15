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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "common.h"
#include "init.h"
#include "dt_signal_util.h"
#include "terminal.h"
#include "error.h"
#include "net.h"

AllMessages allMessages;
void sigWinchHandler(int sigNumber) {
  clearTerminal();
  renderMessages(&allMessages);
}

void handleSocketIo(int signalNumber, siginfo_t *info, void *x) {
  static MessageCodesToBeSentBackQueue messageCodesToBeSentBackAsConfirmationQueue = { .firstElement = NULL, .lastElement = NULL, .size = 0 };

  static int32_t event;
  static int32_t fd;
  //if the signal was received from the same process that
  //sent it, a write operation should be performed
  if(info->si_code == SI_QUEUE) {
    event = POLL_OUT;
    fd = allMessages.socketOutputStatus.fd;
  } else {
    event = info->si_code;
    fd = info->si_fd;
  }

  switch (event) {
    //input from peer available
    case POLL_IN: {
      static char buffer[MAX_MESSAGE_SIZE];
      static int32_t lastReadSize = 0;
      static uint8_t readingStatus = READING_NOTHING;

      lastReadSize = read(fd, buffer, MAX_MESSAGE_SIZE);
      if(lastReadSize == 0) errExit(16);
      while (lastReadSize != -1) {

        for (int32_t i=0; i<lastReadSize; i++) {
          switch (readingStatus) {
            case READING_NOTHING:
              switch (buffer[i]) {
                case BEGIN_OF_MESSAGE_TRANSMISSION:
                  readingStatus = READING_MESSAGES;
                  break;
                case BEGIN_OF_MESSAGES_CONFIRMATIONS:
                  readingStatus = READING_MESSAGES_CONFIRMATIONS;
                  break;
                default:
                  errExit(17);
              }
              break;
            case READING_MESSAGES: {
              static char messageCodeBuffer[4];
              static uint8_t numberOfReadBytesFromMessageCode = 0;
              if(numberOfReadBytesFromMessageCode < 4) {
                messageCodeBuffer[numberOfReadBytesFromMessageCode] = buffer[i];
                numberOfReadBytesFromMessageCode++;
                break;
              }

              static char messageSizeBuffer[2];
              static uint8_t numberOfReadBytesFromMessageSize = 0;
              if(numberOfReadBytesFromMessageSize < 2) {
                messageSizeBuffer[numberOfReadBytesFromMessageSize] = buffer[i];
                numberOfReadBytesFromMessageSize++;
                break;
              }

              static bool gotMessageSize = false;
              static uint16_t messageSize = 0;
              static uint32_t messageCodeInNetworkByteOrder;
              if(!gotMessageSize) {
                gotMessageSize = true;
                memcpy(&messageSize, messageSizeBuffer, 2);
                messageSize = ntohs(messageSize);
                //exiting program here if peer send invalid message size
                if(messageSize > MAX_MESSAGE_SIZE || messageSize <= 0) {
                  errExit(23);
                }
                memcpy(&messageCodeInNetworkByteOrder, messageCodeBuffer, 4);
              }

              //handle this
              static char message[MAX_MESSAGE_SIZE];
              static uint16_t messageReadSize = 0;
              if(messageReadSize < messageSize) {
                if(buffer[i] > 31 && buffer[i] < 127) {
                  message[messageReadSize] = buffer[i];
                } else {
                  errExit(24);
                }
                messageReadSize++;
              }

              if(messageReadSize < messageSize) break;

              //add message to allMessages
              addNewMessage(&allMessages, message, messageSize, MESSAGE_FROM_PEER);
              //add message code to be sent back as confirmation
              enqueueMessageCode(&messageCodesToBeSentBackAsConfirmationQueue, messageCodeInNetworkByteOrder);

              //if it arrived here, the message was read entirely
              //being the case, let's reset values
              numberOfReadBytesFromMessageCode = 0;
              numberOfReadBytesFromMessageSize = 0;
              gotMessageSize = false;
              messageReadSize = 0;
              readingStatus = READING_NOTHING;

              break;
            }
            case READING_MESSAGES_CONFIRMATIONS: {
              static char numberOfConfirmationCodesBuffer[4];
              static uint8_t numberOfReadBytesNumberOfConfirmationCodes = 0;
              if(numberOfReadBytesNumberOfConfirmationCodes < 4) {
                numberOfConfirmationCodesBuffer[numberOfReadBytesNumberOfConfirmationCodes] = buffer[i];
                numberOfReadBytesNumberOfConfirmationCodes++;
                if(numberOfReadBytesNumberOfConfirmationCodes < 4) break;
              }

              static uint32_t numberOfConfirmationCodes = 0;
              static bool gotNumberOfConfirmationCodes = false;
              if(!gotNumberOfConfirmationCodes) {
                gotNumberOfConfirmationCodes = true;
                memcpy(&numberOfConfirmationCodes, numberOfConfirmationCodesBuffer, 4);
                numberOfConfirmationCodes = ntohl(numberOfConfirmationCodes);
                break;
              }

              static uint32_t receivedNumberOfConfirmationsCodes = 0;
              static uint32_t numberOfReadBytesFromMessageConfirmationCodes = 0;
              static char messageConfirmationCodeBuffer[4];
              if(receivedNumberOfConfirmationsCodes < numberOfConfirmationCodes) {
                if(numberOfReadBytesFromMessageConfirmationCodes < 4) {
                  messageConfirmationCodeBuffer[numberOfReadBytesFromMessageConfirmationCodes] = buffer[i];
                  numberOfReadBytesFromMessageConfirmationCodes++;
                  if(numberOfReadBytesFromMessageConfirmationCodes < 4) {
                    break;
                  } else {
                    receivedNumberOfConfirmationsCodes++;

                    static uint32_t messageConfirmationCode = 0;
                    memcpy(&messageConfirmationCode, messageConfirmationCodeBuffer, 4);
                    messageConfirmationCode = ntohl(messageConfirmationCode);
                    if(messageConfirmationCode < 0 || messageConfirmationCode > allMessages.messagesByCode.length) errExit(25);
                    updateSentMessageStatusAsPeerReadByMessageCode(&allMessages, messageConfirmationCode);

                    //if there is more codes to get, go to the next
                    if(receivedNumberOfConfirmationsCodes < numberOfConfirmationCodes) {
                      numberOfReadBytesFromMessageConfirmationCodes = 0;
                      break;
                    }
                  }
                }
              }

              //if it arrived here, all messages confirmation codes were received
              //being the case, let's reset values
              numberOfReadBytesNumberOfConfirmationCodes = 0;
              gotNumberOfConfirmationCodes = false;
              receivedNumberOfConfirmationsCodes = 0;
              numberOfReadBytesFromMessageConfirmationCodes = 0;
              readingStatus = READING_NOTHING;

              break;
            }
          }
        }

        lastReadSize = read(fd, buffer, MAX_MESSAGE_SIZE);
      }

      break;
    }
    //output to peer possible
    case POLL_OUT: {
      allMessages.socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = true;

      static uint8_t writingStatus = WRITING_NOTHING;
      if(writingStatus == WRITING_NOTHING) {
        if(messageCodesToBeSentBackAsConfirmationQueue.size > 0) {
          writingStatus = WRITING_MESSAGES_CONFIRMATIONS;
          allMessages.socketOutputStatus.isThereAnythingBeingSent = true;
        }

        if(allMessages.messagesByCode.length > allMessages.messagesByCode.numberOfSentMessages) {
          writingStatus = WRITING_MESSAGES;
          allMessages.socketOutputStatus.isThereAnythingBeingSent = true;
        }
      }

      switch (writingStatus) {
        case WRITING_MESSAGES_CONFIRMATIONS: {
          bool startedSending = false;
          static char dataToBeSent[9] = { BEGIN_OF_MESSAGES_CONFIRMATIONS };
          
          if(!startedSending) {
            startedSending = true;
            //first byte = protocol code
            //2-5 bytes = number of confirmation codes (Always one in this version of the code)
            //6-9 bytes = confirmation code as unsigned 32 bit number in network byte order
            static uint32_t numberOfConfirmationCodesToSend;
            numberOfConfirmationCodesToSend = htonl(1);
            memcpy(dataToBeSent+1, &numberOfConfirmationCodesToSend, sizeof(uint32_t));

            static uint32_t messageCodeInNetworkByteOrder = 0;
            messageCodeInNetworkByteOrder = dequeueMessageCode(&messageCodesToBeSentBackAsConfirmationQueue);
            memcpy(dataToBeSent+5, &messageCodeInNetworkByteOrder, sizeof(uint32_t));
          }

          static uint8_t writtenSize = 0;
          static int8_t lastWriteSize = 0;
          do {
            lastWriteSize = write(fd, dataToBeSent+writtenSize, 9-writtenSize);
            if(lastWriteSize != -1) writtenSize += lastWriteSize;
          } while (lastWriteSize != -1 && writtenSize < 9);

          if(lastWriteSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            allMessages.socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = false;
            break;
          }

          //if it arrived here, then it means all of the data
          //was sent, that being the case, the values should be reset
          writingStatus = WRITING_NOTHING;
          startedSending = false;
          writtenSize = 0;
          lastWriteSize = 0;
          allMessages.socketOutputStatus.isThereAnythingBeingSent = false;

          break;
        }
        case WRITING_MESSAGES: {
          static bool gotFirstSegment = false;
          static char firstSegmentBuffer[7] = { BEGIN_OF_MESSAGE_TRANSMISSION };

          static Message *messageToBeSent;

          if(gotFirstSegment == false) {
            messageToBeSent = allMessages.messagesByCode.array[allMessages.messagesByCode.numberOfSentMessages];
            gotFirstSegment = true;
            static uint32_t messageCodeInNetworkByteOrder = 0;
            messageCodeInNetworkByteOrder = allMessages.messagesByCode.numberOfSentMessages;
            messageCodeInNetworkByteOrder = htonl(messageCodeInNetworkByteOrder);
            memcpy(firstSegmentBuffer+1, &messageCodeInNetworkByteOrder, 4);
            static uint16_t messageSizeInNetworkByteOrder = 0;
            messageSizeInNetworkByteOrder = htons(messageToBeSent->size);
            memcpy(firstSegmentBuffer+5, &messageSizeInNetworkByteOrder, 2);
          }

          static int8_t lastWriteSize = 0;

          static uint8_t firstSegmentWrittenSize = 0;
          if(firstSegmentWrittenSize < 7) {
            do {
              lastWriteSize = write(fd, firstSegmentBuffer+firstSegmentWrittenSize, 7-firstSegmentWrittenSize);
              if(lastWriteSize != -1) firstSegmentWrittenSize += lastWriteSize;
            } while (lastWriteSize != -1 && firstSegmentWrittenSize < 7);
            if(lastWriteSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
              allMessages.socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = false;
              break;
            }
          }

          static uint16_t messageWrittenSize = 0;
          if(messageWrittenSize < messageToBeSent->size) {
            do {
              lastWriteSize = write(fd, messageToBeSent->string+messageWrittenSize, messageToBeSent->size-messageWrittenSize);
              if(lastWriteSize != -1) messageWrittenSize += lastWriteSize;
            } while (lastWriteSize != -1 && messageWrittenSize < messageToBeSent->size);
            if(lastWriteSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
              allMessages.socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = false;
              break;
            }
          }

          //if it arrived here, then it means all of the data
          //was sent, that being the case, the values should be reset
          writingStatus = WRITING_NOTHING;
          gotFirstSegment = false;
          firstSegmentWrittenSize = 0;
          messageWrittenSize = 0;
          allMessages.socketOutputStatus.isThereAnythingBeingSent = false;

          allMessages.messagesByCode.numberOfSentMessages += 1;

          break;
        }

      }

      break;
    }
    //
  }
}

int main(int argc, char *argv[]) {
  //default values
  allMessages.sizeInChars = 0;
  allMessages.currentStartingMessage = NULL;
  allMessages.currentStartingMessageCharPosition = 0;
  allMessages.lastMessage = NULL;

  allMessages.messagesByCode.array = malloc(sizeof(Message)*DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY);
  allMessages.messagesByCode.availableSpace = DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY;
  allMessages.messagesByCode.currentSize = DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY;
  allMessages.messagesByCode.length = 0;
  allMessages.messagesByCode.numberOfSentMessages = 0;

  allMessages.socketOutputStatus.isThereAnythingBeingSent = false;
  allMessages.socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = true;

  struct sigaction newSigAction;
  memset(&newSigAction, 0, sizeof(struct sigaction));

  newSigAction.sa_handler = &sigWinchHandler;
  sigaction(SIGWINCH, &newSigAction, NULL);

  newSigAction.sa_handler = NULL;
  newSigAction.sa_sigaction = &handleSocketIo;
  newSigAction.sa_flags = SA_SIGINFO;
  sigaction(SIGRTMIN, &newSigAction, NULL);

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
  allMessages.socketOutputStatus.fd = peerConnectedSocket;
  enableSignalDrivenIoOnSocket(peerConnectedSocket, &handleSocketIo);

  struct termios oldTerminalConfigurations;
  memset(&oldTerminalConfigurations, 0, sizeof(struct termios));
  setCbreak(STDIN_FILENO, &oldTerminalConfigurations);

  char inputBuffer[MAX_MESSAGE_SIZE];
  uint16_t inputBufferSize = 0;
  int16_t lastReadSize = 0;
  char processingBuffer[MAX_MESSAGE_SIZE];

  uint8_t applicationMode = EDIT;
  while(true) {
    lastReadSize = read(STDIN_FILENO, processingBuffer, MAX_MESSAGE_SIZE);
    for(int16_t i=0; i<lastReadSize; i++) {
      switch (applicationMode) {
        case EDIT:
          switch (processingBuffer[i]) {
            case ESCAPE:
              applicationMode = VIEW;
              break;
            case DEL:
              if(inputBufferSize > 0) {
                inputBufferSize -= 1;
                renderCurrentlyBeingWrittenMessage(inputBuffer, inputBufferSize);
              }
              break;
            case LINEFEED:
              if(inputBufferSize > 0) {
                blockAllSignalsWithHandlersThatUseMalloc();
                addNewMessage(&allMessages, inputBuffer, inputBufferSize, MESSAGE_FROM_MYSELF);
                unblockAllSignalsWithHandlersThatUseMalloc();
                raise(SIGWINCH);
                inputBufferSize = 0;
              }
              break;
            default:
              //input validation, refer to ascii table
              if(inputBufferSize < MAX_MESSAGE_SIZE && *(processingBuffer+i) > 31 && *(processingBuffer+i) < 127) {
                inputBuffer[inputBufferSize] = processingBuffer[i];
                inputBufferSize += 1;
                renderCurrentlyBeingWrittenMessage(inputBuffer, inputBufferSize);
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
