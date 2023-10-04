#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "init.h"
#include "dt_signal_util.h"

int main(int argc, char *argv[]) {
  Configs configs;
  memset(&configs, 0, sizeof(Configs));
  configs.shouldActAsServer = true;
  if(blockAllSignals() != 0)
    return -1;
  getConfigs(argc, argv, &configs);

  return 0;
}
