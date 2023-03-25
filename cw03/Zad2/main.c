#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    char *name = argv[0];
    char *path = argv[1];
    printf("%s ", name);
    fflush(stdout);
    execl("/bin/ls", "ls", path,NULL);
    return 0;
}
