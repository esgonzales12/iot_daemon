#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

/**
* Estefan Gonzales
* Assignment: Your own Daemon!
* compilation command: gcc -o iotd iotd.c
**/

#define TRUE (1)

static char DAEMON_NAME [] = "iotd";

void signal_handler(const int signal) {
    switch (signal) {
        case SIGHUP:
            break;
        case SIGTERM:
            syslog(LOG_INFO, "iotd exiting on SIGTERM");
            closelog();
            exit(0);
        default:
            syslog(LOG_INFO, "unhandled signal");
    }
}

void iotd_run() {
    while (TRUE) {
        time_t now;
        time(&now);
        syslog(LOG_INFO, "%s", ctime(&now));
        sleep(1);
    }
}

int main() {

    openlog(DAEMON_NAME, LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON);
    syslog(LOG_INFO, "iotd running");
    pid_t pid = fork();

    if (pid < 0) {
        syslog(LOG_ERR, "fork failure");
        return 1;
    } else if (pid > 0) {
        return 0;
    }

    if (setsid() < -1) {
        syslog(LOG_ERR, "error on setsid()");
        return 1;
    }

    close(STDIN_FILENO);
    close(STDERR_FILENO);
    close(STDOUT_FILENO);

    umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (chdir("/") < 0) {
        syslog(LOG_ERR, "unable to choose directory");
        return 1;
    }

    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);

    iotd_run();

    return 1;

}
