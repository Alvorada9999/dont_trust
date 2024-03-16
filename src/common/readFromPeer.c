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

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

#include "common.h"
#include "error.h"

void readFromPeer(AllMessages *allMessages, MessageCodesToBeSentBackQueue *messageCodesToBeSentBackAsConfirmationQueue, int8_t *fd) {
  static char buffer[MAX_MESSAGE_SIZE];
  static int32_t lastReadSize = 0;
  static uint8_t readingStatus = READING_NOTHING;

  lastReadSize = read(*fd, buffer, MAX_MESSAGE_SIZE);
  printf("\n\n------ CALL TO read(), returned value: %d\n\n", lastReadSize);
  fflush(stdout);
  if(lastReadSize == 0) errExit(16);
  while (lastReadSize != -1) {

    for (int32_t i=0; i<lastReadSize; i++) {
      printf("Iteration: %i | Decimal = %d | Char = %c\n", i, buffer[i], buffer[i]);
      fflush(stdout);
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
          addNewMessage(allMessages, message, messageSize, MESSAGE_FROM_PEER);
          if(allMessages->isThereSpaceLeftOnScreenForMoreMessages) {
            // raise(SIGWINCH);
          }
          //add message code to be sent back as confirmation
          enqueueMessageCode(messageCodesToBeSentBackAsConfirmationQueue, messageCodeInNetworkByteOrder);

          printf("\nReceived Message:\n%s\nSize:%u\n\n", message, messageReadSize);
          //if it arrived here, the message was read entirely
          //being the case, let's reset values
          numberOfReadBytesFromMessageCode = 0;
          numberOfReadBytesFromMessageSize = 0;
          gotMessageSize = false;
          messageReadSize = 0;
          readingStatus = READING_NOTHING;

          // renderStatus(MESSAGE_RECEIVED, &allMessages->winSize);

          break;
        }
        case READING_MESSAGES_CONFIRMATIONS: {
          static bool isAnyOfTheMessagesFromCodesOnScreen = false;

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
                if(messageConfirmationCode < 0 || messageConfirmationCode > allMessages->messagesByCode.length) errExit(25);
                updateSentMessageStatusAsPeerReadByMessageCode(allMessages, messageConfirmationCode);
                if(isMessageOnScreen(allMessages, messageConfirmationCode)) {
                  isAnyOfTheMessagesFromCodesOnScreen = true;
                }

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

          if(isAnyOfTheMessagesFromCodesOnScreen) {
            // raise(SIGWINCH);
          }

          // renderStatus(MESSAGE_CONFIRMATION_RECEIVED, &allMessages->winSize);

          break;
        }
      }
    }

    lastReadSize = read(*fd, buffer, MAX_MESSAGE_SIZE);
    printf("\n\n------ CALL TO read(), returned value: %d\n\n", lastReadSize);
    fflush(stdout);

    // printf("\n\n");
    // fflush(stdout);

    // for(int32_t i=0; i<MAX_MESSAGE_SIZE; i++) {
    //   printf("%c", buffer[i]);
    //   fflush(stdout);
    // }
    // printf("\n\n");

    // fflush(stdout);
  }
  allMessages->socketInputStatus.isInputAvailable = false;
}
