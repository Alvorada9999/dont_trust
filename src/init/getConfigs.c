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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "init.h"

bool isValidOnionAddress(char *addr) {
  if(strlen(addr) != 62) {
    return false;
  }
  if(strcmp(addr+55, "d.onion") != 0) {
    return false;
  }
  return true;
}

void getConfigs(int argc, char *argv[], Configs *commandLineOptions){
  char *helpMessage = "This application, if not given an address, will start as a server waiting for connections\n-t   Connects to an IPv4\n-o Connects to an hidden service\n-h   This help message\n";
  int16_t option;
  while((option = getopt(argc, argv, "ht:o:")) != -1) {
    switch (option) {
      case 't': {
        struct in_addr addrptr;
        if(inet_pton(AF_INET, optarg, &addrptr) == 0) {
          printf("Invalid ipv4 address");
          exit(EXIT_SUCCESS);
        }

        option = -1;
        commandLineOptions -> shouldActAsServer = false;
        commandLineOptions -> ipV4 = optarg;
        commandLineOptions -> chosenOption = IPV4_ADDR;
        break;
      }
      case 'o':
        if(isValidOnionAddress(optarg) == false) {
          printf("Invalid onion address\n");
          exit(EXIT_SUCCESS);
        }

        option = -1;
        commandLineOptions -> shouldActAsServer = false;
        commandLineOptions -> onionAddress = optarg;
        commandLineOptions -> chosenOption = ONION_ADDR;
        break;
      case 'h':
        printf("%s", helpMessage);
        exit(EXIT_SUCCESS);
      case '?':
        exit(EXIT_SUCCESS);
    }
  }
}

