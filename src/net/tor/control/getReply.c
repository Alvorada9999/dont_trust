// This file is part of dont_trust.
// Copyright (C) 2024 Kenedy Henrique Bueno Silva

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
#include <unistd.h>
#include <stdbool.h>

#include "error.h"
#include "tor.h"
#include "dt_string_util.h"

//EndReplyLine = 3DIGIT SP [XXXX] CRLF
int32_t getReply(int32_t socketFd, char buffer[65536], char *threeCharLengthStatus) {
  char status[4] = { ' ',' ',' ',' ' };
  bool hasReachedEndOfReply = false, gotStatus = false, gotLastLinePosition = false;
  int32_t lastLinePosition = 0;
  int32_t lastReadSize = 0;
  int32_t totalReadSize = 0;
  while(!hasReachedEndOfReply && totalReadSize < 65536) {
    lastReadSize = read(socketFd, buffer+totalReadSize, 65536-(uint32_t)totalReadSize);
    if(lastReadSize != -1) {
      totalReadSize += lastReadSize;

      if(!gotStatus && totalReadSize > 2) {
        status[0] = buffer[0];
        status[1] = buffer[1];
        status[2] = buffer[2];
        threeCharLengthStatus[0] = buffer[0];
        threeCharLengthStatus[1] = buffer[1];
        threeCharLengthStatus[2] = buffer[2];
        gotStatus = true;
      }

      if(gotStatus && !gotLastLinePosition) {
        lastLinePosition = getIndex(buffer, totalReadSize, status, 4);
        if(lastLinePosition != -1) gotLastLinePosition = true;
      }

      if(gotLastLinePosition && getIndex(buffer+lastLinePosition, totalReadSize-lastLinePosition, "\r\n", 2) != -1) {
        hasReachedEndOfReply = true;
      }

    } else {
      errExit(24);
    }
  }
  if(totalReadSize < 65536) buffer[totalReadSize] = '\0';
  return totalReadSize;
}
