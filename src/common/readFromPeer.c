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

#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

#include "common.h"
#include "error.h"
#include "net.h"
#include "dt_crypto.h"

#define GETTING_MESSAGE_CODE 0
#define GETTING_CIPHER_TEXT_SIZE 1
#define GETTING_INITIALIZATION_VECTOR 2
#define GETTING_SESSION_KEY 3
#define GETTING_PLAINTEXT 4
#define FINALIZING_READING_MESSAGE 5

#define GETTING_NUMBER_OF_MESSAGE_CONFIRMATIONS 6
#define GETTING_MESSAGE_CONFIRMATION 7
#define FINALIZING_READING_MESSAGE_CONFIRMATIONS 8

void readFromPeer(AllMessages *allMessages, MessageCodesToBeSentBackQueue *messageCodesToBeSentBackAsConfirmationQueue, int8_t *fd, EVP_PKEY *pKey) {
  static char buffer[MAX_MESSAGE_SIZE];
  static int32_t lastReadSize = 0;
  static int32_t inputLeftToReadSize;

  while ((lastReadSize = read(*fd, buffer, MAX_MESSAGE_SIZE)) != -1) {
    if(lastReadSize == 0) errExit(16);
    inputLeftToReadSize = lastReadSize;
    static uint8_t readingStatus = READING_NOTHING;

    while(inputLeftToReadSize >= 0) {

      switch (readingStatus) {
        case READING_NOTHING: {
          switch (buffer[lastReadSize-inputLeftToReadSize]) {
            case BEGIN_OF_MESSAGE_TRANSMISSION:
              readingStatus = READING_MESSAGES;
              inputLeftToReadSize--;
              break;
            case BEGIN_OF_MESSAGES_CONFIRMATIONS:
              readingStatus = READING_MESSAGES_CONFIRMATIONS;
              inputLeftToReadSize--;
              break;
            default:
              errExit(17);
          }
          break;
        }
        case READING_MESSAGES: {
          static uint8_t doingNow = GETTING_MESSAGE_CODE;
          static uint32_t gotSize = 0;
          static uint32_t messageCodeInNetworkByteOrder;
          static uint32_t cipherTextSize;
          static char initializationVector[INITIALIZATION_VECTOR_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
          static char sessionKey[SESSION_KEY_LENGTH];
          static char cipherText[MAX_CIPHER_TEXT_LENGTH];
          static char plainText[MAX_MESSAGE_SIZE];
          static uint16_t messageSize = 0;

          switch (doingNow) {
            // ---
            case GETTING_MESSAGE_CODE: {
              static char messageCodeInNetworkByteOrderBuffer[TCP_STREAM_MESSAGE_CODE_INFO_LENGTH/8];
              if(gotSize < TCP_STREAM_MESSAGE_CODE_INFO_LENGTH/8) {
                messageCodeInNetworkByteOrderBuffer[gotSize] = buffer[lastReadSize-inputLeftToReadSize];
                inputLeftToReadSize--;
                gotSize++;
                break;
              } else {
                memcpy(&messageCodeInNetworkByteOrder, messageCodeInNetworkByteOrderBuffer, TCP_STREAM_MESSAGE_CODE_INFO_LENGTH/8);
                doingNow = GETTING_CIPHER_TEXT_SIZE;
                gotSize = 0;
                break;
              }
            }
            case GETTING_CIPHER_TEXT_SIZE: {
              static char cipherTextSizeBuffer[TCP_STREAM_CIPHER_TEXT_SIZE_INFO_LENGTH/8];
              if(gotSize < TCP_STREAM_CIPHER_TEXT_SIZE_INFO_LENGTH/8) {
                cipherTextSizeBuffer[gotSize] = buffer[lastReadSize-inputLeftToReadSize];
                inputLeftToReadSize--;
                gotSize++;
                break;
              } else {
                memcpy(&cipherTextSize, cipherTextSizeBuffer, TCP_STREAM_CIPHER_TEXT_SIZE_INFO_LENGTH/8);
                cipherTextSize = ntohl(cipherTextSize);
                doingNow = GETTING_INITIALIZATION_VECTOR;
                gotSize = 0;
                break;
              }
            }
            case GETTING_INITIALIZATION_VECTOR: {
              if(gotSize < INITIALIZATION_VECTOR_LENGTH) {
                initializationVector[gotSize] = buffer[lastReadSize-inputLeftToReadSize];
                inputLeftToReadSize--;
                gotSize++;
                break;
              } else {
                gotSize = 0;
                doingNow = GETTING_SESSION_KEY;
                break;
              }
            }
            case GETTING_SESSION_KEY: {
              if(gotSize < SESSION_KEY_LENGTH) {
                sessionKey[gotSize] = buffer[lastReadSize-inputLeftToReadSize];
                inputLeftToReadSize--;
                gotSize++;
                break;
              } else {
                gotSize = 0;
                doingNow = GETTING_PLAINTEXT;
                break;
              }
            }
            case GETTING_PLAINTEXT: {
              if(gotSize < cipherTextSize) {
                cipherText[gotSize] = buffer[lastReadSize-inputLeftToReadSize];
                inputLeftToReadSize--;
                gotSize++;
                break;
              } else {
                gotSize = 0;
                messageSize = (uint16_t)envelopeOpen(pKey, (unsigned char *)cipherText, (int32_t)cipherTextSize, (unsigned char *)sessionKey, (unsigned char *)initializationVector, (unsigned char *)plainText);
                doingNow = FINALIZING_READING_MESSAGE;
                break;
              }
            }
            case FINALIZING_READING_MESSAGE: {
              //add message to allMessages
              addNewMessage(allMessages, plainText, messageSize, MESSAGE_FROM_PEER);
              if(allMessages->isThereSpaceLeftOnScreenForMoreMessages) {
                raise(SIGWINCH);
              }
              //add message code to be sent back as confirmation
              enqueueMessageCode(messageCodesToBeSentBackAsConfirmationQueue, messageCodeInNetworkByteOrder);
              renderStatus(MESSAGE_RECEIVED, &allMessages->winSize);

              //if it arrived here, all messages confirmation codes were received
              //being the case, let's reset values
              doingNow = GETTING_MESSAGE_CODE;
              readingStatus = READING_NOTHING;

              //to leave "inputLeftToReadSize" loop
              if(inputLeftToReadSize <= 0) {
                inputLeftToReadSize = -1;
              }
            }
            // ---
          }

          break;
        }
        case READING_MESSAGES_CONFIRMATIONS: {
          static uint8_t doingNow = GETTING_NUMBER_OF_MESSAGE_CONFIRMATIONS;
          static uint32_t gotSize = 0;
          static uint32_t numberOfGotMessageConfirmationCodes = 0;
          static uint32_t numberOfMessageConfirmations = 0;
          static uint32_t messageConfirmationCode = 0;
          static bool isAnyOfTheMessagesFromCodesOnScreen = false;

          switch (doingNow) {
            // ---
            case GETTING_NUMBER_OF_MESSAGE_CONFIRMATIONS: {
              static char numberOfMessageConfirmationsBuffer[sizeof(uint32_t)];
              if(gotSize < sizeof(uint32_t)) {
                numberOfMessageConfirmationsBuffer[gotSize] = buffer[lastReadSize-inputLeftToReadSize];
                inputLeftToReadSize--;
                gotSize++;
                break;
              } else {
                memcpy(&numberOfMessageConfirmations, numberOfMessageConfirmationsBuffer, sizeof(uint32_t));
                numberOfMessageConfirmations = htonl(numberOfMessageConfirmations);
                doingNow = GETTING_MESSAGE_CONFIRMATION;
                gotSize = 0;
                break;
              }
            }
            case GETTING_MESSAGE_CONFIRMATION: {
              static char messageConfirmationBuffer[sizeof(uint32_t)];
              if(gotSize < sizeof(uint32_t)) {
                messageConfirmationBuffer[gotSize] = buffer[lastReadSize-inputLeftToReadSize];
                inputLeftToReadSize--;
                gotSize++;
                break;
              } else {
                numberOfGotMessageConfirmationCodes++;
                memcpy(&messageConfirmationCode, messageConfirmationBuffer, sizeof(uint32_t));
                messageConfirmationCode = ntohl(messageConfirmationCode);

                if(messageConfirmationCode < 0 || messageConfirmationCode > allMessages->messagesByCode.length) errExit(25);
                updateSentMessageStatusAsReceivedReadByMessageCode(allMessages, messageConfirmationCode);
                if(isMessageOnScreen(allMessages, messageConfirmationCode)) {
                  isAnyOfTheMessagesFromCodesOnScreen = true;
                }

                if(numberOfGotMessageConfirmationCodes == numberOfMessageConfirmations) {
                  doingNow = FINALIZING_READING_MESSAGE_CONFIRMATIONS;
                }
                gotSize = 0;
                renderStatus(MESSAGE_CONFIRMATION_RECEIVED, &allMessages->winSize);
                break;
              }
            }
            case FINALIZING_READING_MESSAGE_CONFIRMATIONS: {
              if(isAnyOfTheMessagesFromCodesOnScreen) {
                raise(SIGWINCH);
              }

              //if it arrived here, all messages confirmation codes were received
              //being the case, let's reset values
              numberOfGotMessageConfirmationCodes = 0;
              isAnyOfTheMessagesFromCodesOnScreen = false;
              doingNow = GETTING_NUMBER_OF_MESSAGE_CONFIRMATIONS;
              readingStatus = READING_NOTHING;

              //to leave "inputLeftToReadSize" loop
              if(inputLeftToReadSize <= 0) {
                inputLeftToReadSize = -1;
              }
            }
            // ---
          }

          break;
        }
      }
    }

  }
  allMessages->socketInputStatus.isInputAvailable = false;
}
