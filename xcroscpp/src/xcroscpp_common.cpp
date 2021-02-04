#include "xcroscpp_common.h"

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cassert>
#include <sys/types.h>
#if !defined(WIN32)
#include <unistd.h>
#include <pthread.h>
#endif
#include <signal.h>

using std::string;

void xcros::disableAllSignalsInThisThread()
{
#if !defined(WIN32)
// pthreads_win32, despite having an implementation of pthread_sigmask,
// doesn't have an implementation of sigset_t, and also doesn't expose its
// pthread_sigmask externally.
sigset_t signal_set;

/* block all signals */
sigfillset( &signal_set );
pthread_sigmask( SIG_BLOCK, &signal_set, NULL );
#endif
}