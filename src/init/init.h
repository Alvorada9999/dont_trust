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

#ifndef init
#define init
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <openssl/evp.h>

#define ONION_ADDR 0
#define IPV4_ADDR 1

#define PKEY_PATH_OPTION 'a'
#define PUBKEY_PATH_OPTION 'b'
#define TORCONTROLPASSWORD_PATH_OPTION 'c'

typedef struct {
  char *ipV4;
  char *onionAddress;
  //is true when any of the above is not given
  bool shouldActAsServer;
  int8_t chosenOption;
  EVP_PKEY *pKey;
  EVP_PKEY *pubKey;
  char *torControlPassword;
  int32_t torControlPasswordLength;
} Configs;

#include "common.h"

void getConfigs(int argc, char *argv[], Configs *configsToUpdate);
void enableSignalDrivenIoOnSocket(int8_t socketFd, void (*handler)(int, siginfo_t *, void *));
void setDefaultValues(ProgramData *programData);
void updateBackground(struct winsize *winSize);
void welcomingMessage(void);

#endif // !init
