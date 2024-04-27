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

#ifndef tor
#define tor

#include <stdint.h>
#include <stdbool.h>

#define DEFAULT_TOR_PROXY_PORT 9050
#define DEFAULT_TOR_CONTROL_PORT 9051
#define MAX_POSSIBLE_RESPONSE_SIZE_SOCKS5_CONNECTION_REQUEST 262

int8_t connectToTorSocksProxy(char *onionAddr, uint16_t portNumber);
void sendCommand(int32_t socketFd, char *command, int32_t commandSize);
int32_t getReply(int32_t socketFd, char buffer[65536], char *threeCharLengthStatus);
void getLinePositions(char torReply[65536], int32_t stringLength, int32_t *linesPositionsIndexs, int32_t arrayLength);
bool isStatusEquallsTo(char *status, char *to);
int32_t scapePassword(char *password, int32_t passwordLength, char buffer[2000]);
int8_t startTorServer(char *password, int32_t passwordLength, int8_t *torSockectFd);

#endif // !tor
