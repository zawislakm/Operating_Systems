#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void common() { // ignore and mask common part
    printf("%s\n", "Child process raising signal");
    raise(SIGUSR1);
}


int main(int argc, char *argv[]) {

    printf("%s\n", "Child started!");
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
            common();
            break;
        }
        case 1: // Mask
        {
            common();
            break;
        }

        case 2: // Pending
        {
            sigset_t new_mask;
            sigpending(&new_mask);

            // check whether parent signal is pending
            if (sigismember(&new_mask, SIGUSR1)) {
                printf("Signal pending\n");
            } else {
                printf("Signal NOT pending\n");
            }
            break;
        }

    }

    printf("\n");
}


