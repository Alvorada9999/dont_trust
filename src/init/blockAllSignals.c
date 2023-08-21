#include <signal.h>
#include <stdlib.h>

void blockAllSignals(void) {
  sigset_t sigSetToBlock;
  sigfillset(&sigSetToBlock);
  sigprocmask(SIG_SETMASK, &sigSetToBlock, NULL);
}
