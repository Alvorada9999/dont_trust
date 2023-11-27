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

#define DEFAULT_MESSAGE_OUTPUT_SIZE 140000
#define MAX_MESSAGE_SIZE 65535
#define DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY 1000

enum MessageStatus {
  PEER_NOT_READ = 1,
  PEER_READ = 2,
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

enum ApplicationStatus {
  EDIT = 1,
  VIEW = 2
};

enum MessageOwner {
  MESSAGE_FROM_PEER = 1,
  MESSAGE_FROM_MYSELF = 2
};

struct Message {
  char *string;
  unsigned int size;
  struct Message *nextMessage;
  struct Message *previousMessage;
  uint8_t status;
};

typedef struct Message Message;

typedef struct {
  //the values of those fields are used only by the addNewMessage function in order
  //to know if the signal handler for socket io should be raised
  bool isThereAnythingBeingSent;
  bool isThereAnySpaceOnTheSocketSendBuffer;
  int8_t fd;
} SocketOutputStatus;

typedef struct {
  //this field stores only the messages I sent starging from the first one on the 0 index
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

  MessagesByCodeArray messagesByCode;

  SocketOutputStatus socketOutputStatus;
} AllMessages;

void addNewMessage(AllMessages *allMessages, char *message, uint16_t size, uint8_t owner);
void renderCurrentlyBeingWrittenMessage(char *inputBufer, uint16_t inputBufferSize);

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

#endif // !DTCOMMON
