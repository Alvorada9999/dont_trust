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

#ifndef DTNET
#define DTNET
#include <stdint.h>

#define DEFAULT_SERVER_PORT 9000
#define DEFAULT_TOR_PROXY_PORT 9050

int8_t connectToTorSocksProxy(char *onionAddr, uint16_t portNumber);
int8_t startServer(void);
int8_t simpleConnect(char *addr);

#endif // !DTNET
