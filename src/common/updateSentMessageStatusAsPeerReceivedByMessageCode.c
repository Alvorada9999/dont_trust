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

#include <arpa/inet.h>

#include "common.h"

void updateSentMessageStatusAsReceivedReadByMessageCode(AllMessages *allMessages, uint32_t messageCode) {
  if(messageCode <= allMessages->messagesByCode.currentSize) {
    allMessages->messagesByCode.array[messageCode]->status = PEER_RECEIVED;
  }
}
