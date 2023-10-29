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

enum MessageStatus {
  PEER_NOT_READ = 1,
  PEER_READ = 2,
  RECEIVED = 3
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
  Message *firstMessage;
  Message *LastMessage;
  int size;
} MessageQueue;

typedef struct {
  unsigned int sizeInChars;
  Message *currentStartingMessage;
  unsigned int currentStartingMessageCharPosition;
  Message *messagesInOrder[];
} AllMessages;

enum ApplicationStatus {
  EDIT = 1,
  VIEW = 2
};

void EnqueueMessage(MessageQueue *messageQueue, Message *message);
void DequeueMessage(MessageQueue *messageQueue);

int8_t renderMessages(AllMessages *allMessages);
int8_t updatePostion(AllMessages *allMessages, char jOrK);
u_int16_t getWordSize(char *from, u_int16_t stopAfterNBytes);

#endif // !DTCOMMON
