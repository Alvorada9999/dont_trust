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

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "init.h"
#include "error.h"

void setDefaultValues(ProgramData *programData) {
  programData->sizeInChars = 0;
  programData->currentStartingMessage = NULL;
  programData->currentStartingMessageCharPosition = 0;
  programData->lastMessage = NULL;
  programData->isThereSpaceLeftOnScreenForMoreMessages = true;

  programData->messagesByCode.array = malloc(sizeof(Message*)*DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY);
  if(programData->messagesByCode.array == NULL && errno == ENOMEM) errExit(43);
  programData->messagesByCode.availableSpace = DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY;
  programData->messagesByCode.currentSize = DEFAULT_SIZE_FOR_MESSAGES_BY_CODE_ARRAY;
  programData->messagesByCode.length = 0;
  programData->messagesByCode.numberOfSentMessages = 0;

  programData->socketOutputStatus.isThereAnythingBeingSent = false;
  programData->socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = true;
  programData->socketInputStatus.isInputAvailable = false;

  ioctl(STDIN_FILENO, TIOCGWINSZ, &programData->winSize);
}
