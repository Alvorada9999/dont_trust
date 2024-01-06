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

#include <stdint.h>
#include "common.h"

bool isMessageOnScreen(AllMessages *allMessages, uint32_t messageCode) {
  for(uint16_t i=0; i<DEFAULT_MAX_NUMBER_OF_MESSAGES_ON_SHOW; i++) {
    if(allMessages->messagesBeingShowCode[i] == messageCode) return true;
  }
  return false;
}
