#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "init.h"

int main(int argc, char *argv[]) {
  Configs configs;
  memset(&configs, 0, sizeof(Configs));
  configs.shouldActAsServer = true;
  blockAllSignals();
  getConfigs(argc, argv, &configs);

  return 0;
}
