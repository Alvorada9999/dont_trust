// This file is part of donttrust.
// Copyright (C) 2024 Kenedy Henrique Bueno Silva

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

#include <stdint.h>
#include <unistd.h>

#include "error.h"

void sendCommand(int32_t socketFd, char *command, int32_t commandSize) {
  int32_t lastWriteSize = 0;
  int32_t totalWrittenSize = 0;
  while(totalWrittenSize < commandSize) {
    lastWriteSize = write(socketFd, command+totalWrittenSize, (uint32_t)(commandSize - totalWrittenSize));
    if(lastWriteSize == -1) {
      errExit(23);
    }
    totalWrittenSize += lastWriteSize;
  }
}
