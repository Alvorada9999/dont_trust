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

#ifndef DTNET
#define DTNET
#include <stdint.h>

#define DEFAULT_SERVER_PORT 8000
#define DEFAULT_TOR_SERVER_PORT 80

#define TCP_STREAM_COMMAND_INFO_LENGTH 1
#define TCP_STREAM_MESSAGE_CODE_INFO_LENGTH 32
#define TCP_STREAM_CIPHER_TEXT_SIZE_INFO_LENGTH 32

int8_t startServer(void);
int8_t simpleConnect(char *addr);

#endif // !DTNET
