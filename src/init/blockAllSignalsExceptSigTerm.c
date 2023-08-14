#include <signal.h>
#include <stdlib.h>

void blockAllSignalsExceptSitTerm(void) {
  sigset_t sigSetToBlock;
  sigfillset(&sigSetToBlock);
  sigdelset(&sigSetToBlock, SIGTERM);
  sigprocmask(SIG_SETMASK, &sigSetToBlock, NULL);
}
