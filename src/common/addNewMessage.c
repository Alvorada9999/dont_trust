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

#include "common.h"
#include <stdlib.h>
#include <string.h>

void addNewMessage(AllMessages *allMessages, char *message, uint16_t size) {
  char *text = malloc(size);
  memcpy(text, message, size);

  Message *newMessage = malloc(sizeof(Message));
  memset(newMessage, 0, sizeof(Message));
  newMessage->size = size;
  newMessage->status = PEER_NOT_READ;
  newMessage->string = text;
  newMessage->nextMessage = NULL;

  if(allMessages->sizeInChars > 0) {
    allMessages->lastMessage->nextMessage = newMessage;
    newMessage->previousMessage = allMessages->lastMessage;
  } else {
    newMessage->previousMessage = NULL;
    allMessages->currentStartingMessage = newMessage;
    allMessages->currentStartingMessageCharPosition = 0;
  }

  allMessages->lastMessage = newMessage;
  allMessages->sizeInChars += size;
}
