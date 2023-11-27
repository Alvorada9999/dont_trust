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

#include "common.h"

void enqueueMessageCode(MessageCodesToBeSentBackQueue *queue, uint32_t codeInNetworkByteOrder) {
  MessageCode *messageCode = malloc(sizeof(MessageCode));
  messageCode->next = NULL;
  messageCode->codeInNetworkByteOrder = codeInNetworkByteOrder;

  if(queue->size < 1) {
    queue->firstElement = messageCode;
    queue->lastElement = messageCode;
    queue->size = 1;
  } else {
    queue->lastElement->next = messageCode;
    queue->lastElement = messageCode;
  }
}
