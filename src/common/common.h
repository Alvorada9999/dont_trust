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

#ifndef DTCOMMON
#define DTCOMMON

#include <stdint.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/ioctl.h>

#include <openssl/types.h>

#define UP_BUTTON 'k'
#define DOWN_BUTTON 'j'
#define STATUS_CHANGE_BUTTON 27

//those values are used for handling color customizations on the application
#define STATUS_LINE_TERMINAL_COLOR_CODE "27"
#define PEER_NOT_RECEIVED_TERMINAL_BACKGROUND_COLOR_ID "196"
#define PEER_RECEIVED_TERMINAL_BACKGROUND_COLOR_ID "41"
#define RECEIVED_TERMINAL_BACKGROUND_COLOR_ID "232"
#define TERMINAL_BACKGROUND_COLOR_ID "232"

#define DEFAULT_MESSAGE_OUTPUT_SIZE 140000
//if it's changed, MAX_CIPHER_TEXT_LENGTH should be too
//new MAX_CIPHER_TEXT_LENGTH = (MAX_MESSAGE_SIZE+16)/16)*16
#define MAX_MESSAGE_SIZE 65535
#define DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY 1000
#define DEFAULT_MAX_NUMBER_OF_MESSAGES_ON_SHOW 1000

#define TCP_STREAM_COMMAND_INFO_LENGTH 1
#define TCP_STREAM_MESSAGE_CODE_INFO_LENGTH 32

enum MessageStatus {
  PEER_NOT_RECEIVED = 1,
  PEER_RECEIVED = 2,
  RECEIVED = 3
};

enum SocketIoStatus {
  READING_MESSAGES_CONFIRMATIONS = 1,
  READING_MESSAGES = 2,
  READING_NOTHING = 3,
  WRITING_MESSAGES_CONFIRMATIONS = 4,
  WRITING_MESSAGES = 5,
  WRITING_NOTHING = 6
};

enum ProtocolCodes {
  BEGIN_OF_MESSAGE_TRANSMISSION = 1,
  BEGIN_OF_MESSAGES_CONFIRMATIONS = 2,
  END_OF_MESSAGES_CONFIRMATIONS = 3
};

enum ApplicationModes {
  EDIT = 1,
  VIEW = 2
};

enum MessageOwner {
  MESSAGE_FROM_PEER = 1,
  MESSAGE_FROM_MYSELF = 2
};

enum TypeOfReceivedMessage {
  MESSAGE_SENT = 1,
  MESSAGE_CONFIRMATION_RECEIVED = 2,
  MESSAGE_RECEIVED = 3
};

struct Message {
  char *string;
  unsigned int size;
  struct Message *nextMessage;
  struct Message *previousMessage;
  uint8_t status;
  //Peer messages don't have code
  uint32_t code;
};

typedef struct Message Message;

typedef struct {
  //this value is used only by the addNewMessage function in order
  //to know if the signal handler for socket io should be raised
  bool isThereAnythingBeingSent;

  bool isThereAnySpaceOnTheSocketSendBuffer;
} SocketOutputStatus;

typedef struct {
  bool isInputAvailable;
} SocketInputStatus;

typedef struct {
  //this field stores only the messages I sent starting from the first one on the 0 index
  Message **array;
  uint32_t availableSpace;
  uint32_t currentSize;
  uint32_t length;
  uint32_t numberOfSentMessages;
} MessagesByCodeArray;

typedef struct {
  unsigned int sizeInChars;
  Message *currentStartingMessage;
  unsigned int currentStartingMessageCharPosition;
  Message *lastMessage;

  // --- when adding message & when receiving message ---
  bool isThereSpaceLeftOnScreenForMoreMessages;
  // ----------------------------------------------------

  // --- when receiving confirmation ---
  uint16_t numberOfMessagesBeingShow;
  uint32_t messagesBeingShowCode[DEFAULT_MAX_NUMBER_OF_MESSAGES_ON_SHOW];
  // -----------------------------------

  //this field stores only the messages I sent
  MessagesByCodeArray messagesByCode;

  SocketOutputStatus socketOutputStatus;
  SocketInputStatus socketInputStatus;

  struct winsize winSize;

  EVP_PKEY *pKey;
  EVP_PKEY *pubKey;
} AllMessages;

void addNewMessage(AllMessages *allMessages, char *message, uint16_t size, uint8_t owner);
void renderCurrentlyBeingWrittenMessage(char *inputBufer, uint16_t inputBufferSize, struct winsize *winSize);

int8_t renderMessages(AllMessages *allMessages);
int8_t updatePostion(AllMessages *allMessages, char jOrK);
u_int16_t getWordSize(char *from, u_int16_t stopAfterNBytes);

void updateSentMessageStatusAsPeerReadByMessageCode(AllMessages *allMessages, uint32_t messageCode);

struct MessageCode {
  uint32_t codeInNetworkByteOrder;
  struct MessageCode *next;
};

typedef struct MessageCode MessageCode;

typedef struct {
  //both in network byte order
  MessageCode *firstElement;
  MessageCode *lastElement;
  uint32_t size;
} MessageCodesToBeSentBackQueue;

void enqueueMessageCode(MessageCodesToBeSentBackQueue *queue, uint32_t code);
uint32_t dequeueMessageCode(MessageCodesToBeSentBackQueue *queue);
bool isMessageOnScreen(AllMessages *allMessages, uint32_t messageCode);

void processInput(AllMessages *allMessages);
void writeToPeer(AllMessages *allMessages, MessageCodesToBeSentBackQueue *messageCodesToBeSentBackAsConfirmationQueue, int8_t *fd, EVP_PKEY *pubKey);
#include "init.h"
void readFromPeer(AllMessages *allMessages, MessageCodesToBeSentBackQueue *messageCodesToBeSentBackAsConfirmationQueue, int8_t *fd, EVP_PKEY *pKey);

void renderStatus(uint8_t type, struct winsize *winSize);

#endif // !DTCOMMON
