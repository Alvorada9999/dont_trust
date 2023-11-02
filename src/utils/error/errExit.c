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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "error.h"

void errExit(uint16_t errCode) {
  char *errors[20];
  errors[1] = "Error when blocking all signals\n";
  errors[2] = "Error unblocking int and winch signals\n";
  errors[3] = "Error setting terminal in break state\n";
  errors[4] = "Amount of chars that can be show when rendering messsages is greater than DEFAULT_MESSAGE_OUTPUT_SIZE\n";

  errors[5] = "Invalid length for onion address\n";
  errors[6] = "Tor socks proxy did not provided a supported authentication method as response for the gretting\n";
  errors[7] = "Proxy connection request error: General failure\n";
  errors[8] = "Proxy connection request errror: Connection not allowed by ruleset\n";
  errors[9] = "Proxy connection request error: Network unreachable\n";
  errors[10] = "Proxy connection request error: Host unreachable\n";
  errors[11] = "Proxy connection request error: Connection refused by destination host\n";
  errors[12] = "Proxy connection request error: TTL expired\n";
  errors[13] = "Proxy connection request error: Command not supported / protocol error\n";
  errors[14] = "Proxy connection request error: Address type not supported\n";
  errors[15] = "Error when trying to connect socket program tcp socket to tor socks proxy\n";

  printf("%s", errors[errCode]);
  fflush(stdout);

  exit(errCode);
}
