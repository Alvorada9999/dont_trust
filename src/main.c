#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "init.h"

int main(int argc, char *argv[]) {
  CommandLineOptions commandLineOptions;
  memset(&commandLineOptions, 0, sizeof(CommandLineOptions));
  
  blockAllSignalsExceptSigTerm();
  getCommandLineOptions(argc, argv, &commandLineOptions);
  printf("IPv4: %s\n", commandLineOptions.ipV4);

  return 0;
}
