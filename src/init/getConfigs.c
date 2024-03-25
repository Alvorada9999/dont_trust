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
#include <getopt.h>

#include "init.h"
#include "dt_crypto.h"

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
  const char *helpMessage = "This application, if not given an address, will start as a server waiting for connections\n\n  -t            Connects to an IPv4\n  -o            Connects to a hidden service\n  --pKey        Your PEM encoded private key file path\n  --pubKey      The peer PEM encoded public key file path\n  -h            This help message\n";
  commandLineOptions -> ipV4 = NULL;
  commandLineOptions -> onionAddress = NULL;
  commandLineOptions -> chosenOption = -1;
  commandLineOptions -> pKey = NULL;
  commandLineOptions -> pubKey = NULL;

  struct option longopts[3];
  longopts[0].name = "pKey";
  longopts[0].has_arg = required_argument;
  longopts[0].flag = NULL;
  longopts[0].val = PKEY_PATH_OPTION;
  longopts[1].name = "pubKey";
  longopts[1].has_arg = required_argument;
  longopts[1].flag = NULL;
  longopts[1].val = PUBKEY_PAPTH_OPTION;
  longopts[2].name = NULL;
  longopts[2].has_arg = 0;
  longopts[2].flag = NULL;
  longopts[2].val = 0;
  int32_t longindex;
  int32_t option = 0;
  while((option = getopt_long_only(argc, argv, "ht:o:", longopts, &longindex)) != -1) {
    switch (option) {
      case 't': {
        if(commandLineOptions -> chosenOption == ONION_ADDR) {
          printf("Onion address already given\n");
          exit(EXIT_SUCCESS);
        }
        struct in_addr addrptr;
        if(inet_pton(AF_INET, optarg, &addrptr) == 0) {
          printf("Invalid ipv4 address\n");
          exit(EXIT_SUCCESS);
        }

        commandLineOptions -> shouldActAsServer = false;
        commandLineOptions -> ipV4 = optarg;
        commandLineOptions -> chosenOption = IPV4_ADDR;
        break;
      }
      case 'o':
        if(commandLineOptions -> chosenOption == IPV4_ADDR) {
          printf("IPV4 address already given\n");
          exit(EXIT_SUCCESS);
        }
        if(isValidOnionAddress(optarg) == false) {
          printf("Invalid onion address\n");
          exit(EXIT_SUCCESS);
        }

        commandLineOptions -> shouldActAsServer = false;
        commandLineOptions -> onionAddress = optarg;
        commandLineOptions -> chosenOption = ONION_ADDR;
        break;
      case 'h':
        printf("%s", helpMessage);
        fflush(stdout);
        break;
      case PKEY_PATH_OPTION:
        commandLineOptions -> pKey = createPrivateKeyFromFilePath(optarg);
        break;
      case PUBKEY_PAPTH_OPTION:
        commandLineOptions -> pubKey = createPublicKeyFromFilePath(optarg);
        break;
      case '?':
        exit(EXIT_SUCCESS);
    }
  }
  if(commandLineOptions -> pKey == NULL || commandLineOptions -> pubKey == NULL) {
    printf("Both private keys should be provided\n");
    exit(EXIT_SUCCESS);
  }
}

