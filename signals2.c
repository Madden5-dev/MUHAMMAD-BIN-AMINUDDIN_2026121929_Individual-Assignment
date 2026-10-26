#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig)
{
    if (sig == SIGINT) {
        printf("This is a special signal handler for SIGINT\n");
    } else if (sig == SIGTSTP) {
        printf("This is a special signal handler for SIGTSTP\n");
    } else if (sig == SIGQUIT) {
        printf("This is a special signal handler for SIGQUIT\n");
        printf("Exiting program now...\n");
        exit(0);
    }
}

int main(void)
{
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(1);
    }
    if (signal(SIGTSTP, signal_handler) == SIG_ERR) {
        perror("signal SIGTSTP");
        exit(1);
    }
    if (signal(SIGQUIT, signal_handler) == SIG_ERR) {
        perror("signal SIGQUIT");
        exit(1);
    }

    printf("Press Ctrl+C, Ctrl+Z, or Ctrl+\\ to test (Ctrl+\\ will exit):\n");

    while(1) {
        sleep(1);
    }

    return 0;
}
