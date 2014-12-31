#ifndef __API_DARWIN_CONFIG_H_INCLUDED__
#define __API_DARWIN_CONFIG_H_INCLUDED__

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stddef.h>             /* offsetof() */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <glob.h>
#include <sys/mount.h>          /* statfs() */

#include <sys/filio.h>          /* FIONBIO */
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sched.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>        /* TCP_NODELAY */
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>

#include <sys/sysctl.h>
#include <xlocale.h>


#ifndef IOV_MAX
#define IOV_MAX   64
#endif


//#include <ngx_auto_config.h>


#if (API_HAVE_POSIX_SEM)
#include <semaphore.h>
#endif


#if (API_HAVE_POLL)
#include <poll.h>
#endif


#if (API_HAVE_KQUEUE)
#include <sys/event.h>
#endif


#define API_LISTEN_BACKLOG  -1

#ifndef API_HAVE_INHERITED_NONBLOCK
#define API_HAVE_INHERITED_NONBLOCK  1
#endif


#ifndef API_HAVE_CASELESS_FILESYSTEM
#define API_HAVE_CASELESS_FILESYSTEM  1
#endif


#define API_HAVE_OS_SPECIFIC_INIT    1
#define API_HAVE_DEBUG_MALLOC        1


extern char **environ;


#endif /* ! __API_DARWIN_CONFIG_H_INCLUDED__ */
