#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int flag = 1;

void handler(int signo) {
    flag = -1;
}


int main(int argc, char *argv[]) {
    if (argc < 3)
        exit(1);



    int catcher_pid = atoi(argv[1]);

    for (int i = 2; i < argc; i++) {
        flag = 1;

        int to_send = atoi(argv[i]);
        if (to_send < 1 || to_send > 5)
            continue;

        struct sigaction act;
        sigemptyset(&act.sa_mask);
        act.sa_handler = handler;
        sigaction(SIGUSR1, &act, NULL);

        union sigval sig_val;
        sig_val.sival_int = to_send;
        sigqueue(catcher_pid, SIGUSR1, sig_val);
        printf("Sent signal %d\n", to_send);
        while (flag != -1) {
            printf("Waiting for signal back\n");
            sleep(5);
        }

        if (flag == -1) {
            printf("Signal back received\n");
        }
    }
    return 0;
}
