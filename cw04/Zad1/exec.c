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

    if (flag == 2) { //only pending requires checking whether signal is pending
        if (sigismember(&new_mask, SIGUSR1)) {
            printf("Signal pending\n");
        } else {
            printf("Signal NOT pending\n");
        }
    }
}


int main(int argc, char *argv[]) {

    int flag; // 0 ignore,1 mask, 2 pending

    if (strcmp(argv[1], "ignore") == 0) {
        flag = 0;
    } else if (strcmp(argv[1], "mask") == 0) {
        flag = 1;
    } else if (strcmp(argv[1], "pending") == 0) {
        flag = 2;
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

            execl("./exec_child", "./exec_child", "ignore", NULL);
            break;
        }
        case 1: // Mask
        {
            printf("%s\n", "Mask!");
            common(flag);
            execl("./exec_child", "./exec_child", "mask", NULL);

            break;
        }
        case 2: // Pending
        {
            printf("%s\n", "Pending!");
            common(flag);
            execl("./exec_child", "./exec_child", "pending", NULL);
        }

    }
    printf("\n");

}