#include <signal.h>
#include <stdlib.h>

int unblockIntAndWinchSignals(void) {
  sigset_t sigSet;
  if(sigemptyset(&sigSet) == -1)
    return -2;
  if(sigaddset(&sigSet, SIGINT) == -1)
    return -2;
  if(sigaddset(&sigSet, SIGWINCH) == -1)
    return -2;
  if(sigprocmask(SIG_UNBLOCK, &sigSet, NULL) == -1)
    return -2;
  return 0;
}
