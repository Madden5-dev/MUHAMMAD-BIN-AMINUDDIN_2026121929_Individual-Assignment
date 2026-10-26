#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t parent_pid;

// Signal handler for SIGINT (Ctrl + C) - Only parent prints the message
void sigint_handler(int sig) {
    if (getpid() == parent_pid) {
        printf("\n[SIGINT caught] Program Stopped.\n");
    }
    exit(0);
}

// Function to check if a number is prime
int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(void) {
    int pipefds[2];
    pid_t pid;
    int num;

    // Save the parent's PID before forking
    parent_pid = getpid();

    // Register signal handler for SIGINT
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(EXIT_FAILURE);
    }

    // Create pipe
    if (pipe(pipefds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process: asks for input and sends via pipe
        close(pipefds[0]); // Close unused read end

        printf("Child process - Please enter an integer: ");
        if (scanf("%d", &num) != 1) {
            exit(EXIT_FAILURE);
        }

        write(pipefds[1], &num, sizeof(num));
        close(pipefds[1]); // Close write end
        exit(EXIT_SUCCESS);
    } else {
        // Parent process: reads from pipe, checks prime, and prints result
        close(pipefds[1]); // Close unused write end

        wait(NULL); // Wait for child to finish prompt & write

        read(pipefds[0], &num, sizeof(num));
        close(pipefds[0]); // Close read end

        if (is_prime(num)) {
            printf("Parent process: %d is a Prime number.\n", num);
        } else {
            printf("Parent process: %d is NOT a Prime number.\n", num);
        }
    }

    return EXIT_SUCCESS;
}
