#include <signal.h>
#include <stdlib.h>

int blockAllSignals(void) {
  sigset_t sigSetToBlock;
  if(sigfillset(&sigSetToBlock) != 0)
    return -1;
  if(sigprocmask(SIG_SETMASK, &sigSetToBlock, NULL) != 0)
    return -1;
  return 0;
}
