#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void handler() {
    printf("Normal handler worked\n");
}


void handler_info(int signo, siginfo_t *info, void *context) {
    printf("Info handler worked\n");
    printf("Signal %d, Code %d, Pid %d\n", signo, info->si_code, info->si_pid);
}

void child_work(int signal_flag) {
    printf("Child process raising signal!\n");
    raise(signal_flag);
    printf("Child process signal raised!\n");
}

int main(int argc, char *argv[]) {
    int flag;
    if (strcmp(argv[1], "SA_SIGINFO") == 0) {
        flag = 0;
    } else if (strcmp(argv[1], "SA_NOCLDSTOP") == 0) {
        flag = 1;
    } else if (strcmp(argv[1], "SA_NOCLDWAIT") == 0) {
        flag = 2;
    } else {
        puts("WRONG OPTION!");
        return 0;
    }
    struct sigaction saction;
    switch (flag) {
        case 0: { //siginfo
            printf("SA_SIGINFO!\n");

            saction.sa_flags = SA_SIGINFO;
            saction.sa_sigaction = handler_info;
            sigaction(SIGUSR1, &saction, NULL);

            child_work(SIGUSR1);
            return 0;
        }
        case 1: { //nocldstop
            printf("SA_NOCLDSTOP!\n");
            saction.sa_flags = SA_NOCLDSTOP;
            saction.sa_sigaction = handler;
            printf("Stop child process, handler only in parent\n");
            sigaction(SIGCHLD, &saction, NULL);
            break;
        }
        case 2: {
            printf("SA_NOCLDWAIT!\n");
            saction.sa_flags = SA_NOCLDWAIT;
            saction.sa_sigaction = handler;
            sigaction(SIGCHLD, &saction, NULL);
            break;
        }

    }

    pid_t pid = fork();
    if (pid == -1) {
        printf("Fork error");
        exit(1);
    }

    if (pid == 0) { //proces potomny
        child_work(SIGSTOP);

    }

    if (pid != 0) {
        switch (flag) {
            case 1: { // stop
                sigset_t set;
                sigpending(&set);

                //zabicie dziecka
                kill(pid, SIGKILL);
                wait(NULL);
                break;
            }
            case 2: { //wait
                if ((waitpid(pid, NULL, 0)) == -1) {
                    printf("Zombie child\n");

                    //zabicie dziecka
                    kill(pid, SIGKILL);
                    break;
                }
            }
        }
    }

    printf("\n");
    return 0;

}


/*

    SA_SIGINFO (since Linux 2.2)
              Ustawienie tej flagi jest przydatne w przypadku,
              gdy funkcja obsługi sygnału potrzebuje bardziej szczegółowych informacji o
              sygnale niż tylko jego numer. Na przykład, gdy sygnał jest wysyłany przez inny proces,
              funkcja obsługi sygnału może użyć pola PID w strukturze siginfo_t do
              identyfikacji procesu, który wysłał sygnał.

    SA_NOCLDWAIT (since Linux 2.6)
              Ustawienie flagi SA_NOCLDWAIT może prowadzić do sytuacji,
              w której proces potomny stanie się zombi (czyli będzie miał status Z) w systemie,
              jeśli proces macierzysty nie wywoła funkcji wait() w celu odebrania statusu procesu potomnego.


    SA_NOCLDSTOP
              W przypadku ustawienia flagi SA_NOCLDSTOP, proces macierzysty nie
              otrzymuje tego sygnału i nie wywołuje funkcji obsługi sygnału.
 */



