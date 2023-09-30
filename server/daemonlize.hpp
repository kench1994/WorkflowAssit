#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

static int set_rlimit(int resource, rlim_t current, rlim_t max)
{
    struct rlimit rlim;
    int res;

    rlim.rlim_cur = current;
    rlim.rlim_max = max;

    res = setrlimit(resource, &rlim);
    return res;
}

int daemonize()
{
    // fork to run in background
    int pid = fork();
    switch (pid)
    {
    case -1:
        fprintf(stderr, "fork failed: %s", strerror(errno));
        return -1;
    case 0:
        break;
    default:
        exit(EXIT_SUCCESS);
    }

    // become session leader to lose controlling TTY
    setsid();

    // get maximun number of file descriptors
    struct rlimit rl;
    if (0 > getrlimit(RLIMIT_NOFILE, &rl))
        return -1;

    // close all open file descriptors
    for (int i = 0; i < rl.rlim_max; i++)
        close(i);

    set_rlimit(RLIMIT_NOFILE, RLIM_INFINITY, RLIM_INFINITY);
    set_rlimit(RLIMIT_CORE,   RLIM_INFINITY, RLIM_INFINITY);

    // reopen stdin, stdout and stderr
    int stdin = open("/dev/null", O_RDWR);
    int stdout = dup(stdin);
    int stderr = dup(stdin);
    if (stdin != 0 || stdout != 1 || stderr != 2)
        return -1;

    // change the current working directory to root
    // in order not to prevent file system from being umounted
    if (chdir("/") < 0)
        return -1;

    // clear file creation mask
    umask(0);

    // TODO run as another user

    // TODO create PID

    return 0;
}