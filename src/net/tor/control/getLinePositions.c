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

void getLinePositions(char torReply[65536], int32_t stringLength, int32_t *linesPositionsIndexs, int32_t arrayLength) {
  if(arrayLength <= 1) return;
  //the 0 index stores the number of lines
  linesPositionsIndexs[0] = 1;
  //each following i position stores the index of the i line
  linesPositionsIndexs[1] = 0;
  int32_t leftSpaceOnArray = arrayLength-2;
  int32_t currentLineToGet = 2;
  for(int32_t i=0; i<stringLength && leftSpaceOnArray>0; i++) {
    if(torReply[i]=='\r' && i+1<stringLength && torReply[i+1]=='\n' && i+2<stringLength) {
      linesPositionsIndexs[currentLineToGet] = i+2;
      linesPositionsIndexs[0] = currentLineToGet;
      currentLineToGet++;
      leftSpaceOnArray--;
    }
  }
}
