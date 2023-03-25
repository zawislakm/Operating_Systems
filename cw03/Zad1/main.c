#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    int n = atoi(argv[1]);

    if (n < 0){
        return 0;
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork failed\n");
            exit(1);
        } else if (pid == 0) {
            printf("%d,%d\n", getppid(), getpid());
            exit(0);
        }
    }


    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    printf("%d\n", n);
    return 0;
}
