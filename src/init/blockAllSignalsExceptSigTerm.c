#include <signal.h>
#include <stdlib.h>

void blockAllSignalsExceptSigTerm(void) {
  sigset_t sigSetToBlock;
  sigfillset(&sigSetToBlock);
  sigdelset(&sigSetToBlock, SIGTERM);
  sigprocmask(SIG_SETMASK, &sigSetToBlock, NULL);
}
