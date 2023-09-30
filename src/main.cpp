#include "server.hpp"
#include "daemonlize.hpp"


static int sigterm_signaled = 0;
static int sigintr_signaled = 0;
static int sigalrm_signaled = 0;

static void sig_handler(int signo, siginfo_t *siginfo, void *ucontext)
{
    switch (signo)
    {
    case SIGINT:
        sigintr_signaled = 1;
        break;
    case SIGTERM:
        sigterm_signaled = 1;
        break;
    case SIGQUIT:
        break;
    case SIGALRM:
        sigalrm_signaled = 1;
        break;
    default:
        break;
    }
    return;
}

int install_signals()
{
    int handle_signals[] = {SIGINT, SIGTERM, SIGQUIT, SIGALRM, SIGIO};
    int ignore_signals[] = {SIGHUP, SIGUSR1, SIGUSR2, SIGPIPE};

    struct sigaction sa;
    for (size_t i = 0; i < sizeof(handle_signals) / sizeof(int); i++)
    {
        memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = sig_handler;
        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        if (sigaction(handle_signals[i], &sa, NULL) == -1)
        {
            syslog(LOG_ERR, "sigaction(%d) failed: %s", handle_signals[i], strerror(errno));
            return -1;
        }
    }

    for (size_t i = 0; i < sizeof(ignore_signals) / sizeof(int); i++)
    {
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = SIG_IGN;
        sigemptyset(&sa.sa_mask);
        if (sigaction(ignore_signals[i], &sa, NULL) == -1)
        {
            syslog(LOG_ERR, "sigaction(%d) failed: %s", ignore_signals[i], strerror(errno));
            return -1;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
	daemonize();

	server s("tcp://*:5570");
	s.initial();


    openlog("worker_assit", LOG_PID | LOG_NDELAY, LOG_LOCAL0);

    // set up signal handler
    if (install_signals() != 0)
    {
        syslog(LOG_ERR, "register_signals failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if (sigterm_signaled || sigintr_signaled || sigalrm_signaled)
            break;
		s.work();
    }

    return 0;
}
