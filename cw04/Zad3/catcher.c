#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

int count = 0;

int flag = -1;

void signal1() {
    for (int i = 1; i < 101; i++) {
        printf("%d\n", i);
    }
    flag = -1;
}

void signal2() {
    time_t t = time(NULL);
    char *current_time = ctime(&t);
    printf("Current time: %s\n", current_time);
    flag = -1;
}

void signal3() {
    printf("Changed state: %d\n", count);
    flag = -1;
}

void signal4() {

    signal2();
    sleep(1);
    flag = 4;

}

void handler(int signo, siginfo_t *info, void *context) {
    int pid_sender = info->si_pid;
    int signal_sent = info->si_status;

    flag = signal_sent;
    count += 1;
    kill(pid_sender, SIGUSR1);
}

int main() {
    int pid = getpid();

    printf("%d\n", getpid());

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    while (1) {
        switch (flag) {
            case 1:
                signal1();
                break;
            case 2:
                signal2();
                break;
            case 3:
                signal3();
                break;
            case 4:
                signal4();
                break;
            case 5:
                printf("End work!\n");
                exit(1);
            default:
                break;
        }
        printf("[iteration]\n");
        sleep(1);
    }

}
