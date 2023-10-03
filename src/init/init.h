#ifndef init
#define init
#include <stdbool.h>

typedef struct {
  char *ipV4;
  //is true when the above is not given
  bool shouldActAsServer;
} Configs;

void getConfigs(int argc, char *argv[], Configs *commandLineOptions);

#endif // !init
