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
#include <string.h>
#include <stdbool.h>

#include "init.h"

bool isLineOptionValid(char *string, char *array[]) {
  for(int i=0; i<2; i++) {
    if(strcmp(string, array[i]) == 0) {
      return true;
    }
  }
  return false;
}

void getConfigs(int argc, char *argv[], Configs *commandLineOptions){
  char *comandLineOptions[] = {"-h\0", "-c\0"};
  char *helpMessage = "This application, if not given an address, will start as a server waiting for connections\n-c   Connects to an IPv4\n-h   This help message\n";
  for(int i=0; i<argc; i++) {
    if(argv[i][0] == '-' && isLineOptionValid(argv[i], comandLineOptions) == false) {
      printf("\nInvalid line option \"%s\"\n", argv[i]);
      exit(EXIT_SUCCESS);
    }
    if(strcmp(argv[i], comandLineOptions[0]) == 0) {
      printf("%s", helpMessage);
      exit(EXIT_SUCCESS);
    } else if(strcmp(argv[i], comandLineOptions[1]) == 0) {
      if(argv[i+1] == NULL) {
        printf("Should provide IPv4 address\n");
        exit(EXIT_SUCCESS);
      }
      commandLineOptions -> shouldActAsServer = false;
      commandLineOptions -> ipV4 = argv[i+1];
    }
  }
}

