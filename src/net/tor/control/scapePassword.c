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
#include <stdbool.h>

// backslashes and double quotes only
int32_t scapePassword(char *password, int32_t passwordLength, char buffer[2000]) {
  int32_t currentBufferPosition = 0;
  for(int32_t i=0; i<passwordLength; i++) {
    if(password[i] == '\\' || password[i] == '"') {
      buffer[currentBufferPosition] = '\\';
      buffer[currentBufferPosition+1] = password[i];
      currentBufferPosition += 2;
    } else {
      buffer[currentBufferPosition] = password[i];
      currentBufferPosition++;
    }
  }
  return currentBufferPosition;
}
