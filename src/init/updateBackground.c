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

#include <stdio.h>

#include "init.h"
#include "common.h"

void updateBackground(struct winsize *winSize) {
  printf("\033[48;5;%sm\033[H\033[0J", TERMINAL_BACKGROUND_COLOR_ID);
  fflush(stdout);
  renderStatus(0, winSize);
  printf("\033[H");
  fflush(stdout);
}
