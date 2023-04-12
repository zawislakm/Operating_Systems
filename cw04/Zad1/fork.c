#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>


void common(int flag) { //common part mask && pending
    sigset_t new_mask;
    sigset_t current_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &new_mask, &current_mask);

    printf("%s\n", "Parent raising signal");
    raise(SIGUSR1);

    if (flag == 3) { //only pending requires checking whether signal is pending
        if (sigismember(&new_mask, SIGUSR1)) {
            printf("Signal pending\n");
        } else {
            printf("Signal NOT pending\n");
        }
    }
}

void handler(int sig_no) {
    printf("Received signal, handler, PID: %d, PPID: %d\n", getpid(), getppid());
}


int main(int argc, char *argv[]) {

    int flag; // 0 ignore, 1 handler, 2 mask, 3 pending

    if (strcmp(argv[1], "ignore") == 0) {
        flag = 0;
    } else if (strcmp(argv[1], "handler") == 0) {
        flag = 1;
    } else if (strcmp(argv[1], "mask") == 0) {
        flag = 2;
    } else if (strcmp(argv[1], "pending") == 0) {
        flag = 3;
    } else {
        puts("WRONG OPTION!");
        return 0;
    }

    switch (flag) {
        case 0: // Ignore
        {
            printf("%s\n", "Ignore!");
            signal(SIGUSR1, SIG_IGN);
            printf("%s\n", "Parent raising signal");
            raise(SIGUSR1);

            pid_t pid = fork();
            if (pid == 0) {
                printf("%s\n", "Child started!");
                printf("%s\n", "Child process raising signal");
                raise(SIGUSR1);
            } else {
                wait(NULL);
            }
            break;
        }
        case 1: // Handler
        {
            printf("%s\n", "Handler!");
            signal(SIGUSR1, handler);
            printf("%s\n", "Parent raising signal");
            raise(SIGUSR1);

            pid_t pid = fork();
            if (pid == 0) {
                printf("%s\n", "Child started!");
                printf("%s\n", "Child process raising signal");
                raise(SIGUSR1);
            } else {
                wait(NULL);
            }
            break;
        }

        case 2: // Mask
        {
            printf("%s\n", "Mask!");
            common(flag);

            pid_t pid = fork();
            if (pid == 0) {
                // if "pending" mode then don't raise a signal in child process
                printf("%s\n", "Child started!");
                printf("%s\n", "Child process raising signal");
                raise(SIGUSR1);

            } else {
                wait(NULL);
            }

            break;
        }
        case 3: // Pending
        {
            printf("%s\n", "Pending!");
            common(flag);

            pid_t pid = fork();
            if (pid == 0) {
                printf("%s\n", "Child started!");
                sigset_t new_mask;
                sigpending(&new_mask);

                if (sigismember(&new_mask, SIGUSR1)) {
                    printf("Signal pending\n");
                } else {
                    printf("Signal NOT pending\n");
                }
            } else {
                wait(NULL);
            }

            break;
        }
    }

    printf("\n");
    return 0;
}
