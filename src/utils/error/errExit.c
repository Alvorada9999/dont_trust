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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "error.h"

void errExit(uint16_t errCode) {
  char *errors[44];
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

  errors[16] = "Peer closed the connection\n";
  errors[17] = "Invalid protocol option received\n";

  errors[18] = "Error when trying to create socket\n";
  errors[19] = "Error when trying to bind address to socket\n";
  errors[20] = "Error when trying to listen on port\n";
  errors[21] = "Error when trying to accept connection\n";
  errors[22] = "Error when trying to establishing connection\n";

  errors[25] = "Peer sent invalid message confirmation code\n";

  errors[26] = "Failed to read private key file\n";
  errors[27] = "Failed to read pem encoded private key\n";
  errors[28] = "Failed to read public key file\n";
  errors[29] = "Failed to read pem encoded public key\n";

  errors[30] = "Failed to initialize context for decryption";
  errors[31] = "Failed to initialize the decryption operation";
  errors[32] = "Failed to perform the decryption operation";
  errors[33] = "Failed to finalize the decryption operation";

  errors[34] = "Failed to initialize context for encryption";
  errors[35] = "Failed to initialize the encryption operation";
  errors[36] = "Failed to perform the encryption operation";
  errors[37] = "Failed to finalize the encryption operation";

  errors[23] = "Error when sending command to tor\n";
  errors[24] = "Error when receiving reply from tor\n";
  errors[38] = "Error when trying to connect to tor\n";
  errors[39] = "PROTOCOLINFO command responded with an error code\n";
  errors[41] = "PROTOCOLINFO command responded with an unsupported PIPEVERSION\n";
  errors[40] = "AUTHENTICATE command responded with an error code\n";
  errors[42] = "ADD_ONION command responded with an error code\n";

  errors[43] = "Not enough memory\n";

  printf("\033[0m\033[H\033[0J\033[3J%s", errors[errCode]);
  exit(errCode);
}
