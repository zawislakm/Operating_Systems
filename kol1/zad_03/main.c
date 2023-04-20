#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Not a suitable number of program parameters\n");
        exit(1);
    }

    int toChildFD[2];
    int toParentFD[2];

    if (pipe(toChildFD) == -1 || pipe(toParentFD) == -1) {
        printf("Pipe error\n");
        exit(1);
    }

    int val1, val2, val3 = 0;


    pid_t pid;
    if ((pid = fork()) == 0) {

        //odczytaj z potoku nienazwanego wartosc przekazana przez proces macierzysty i zapisz w zmiennej val2
        close(toChildFD[1]);
        if (read(toChildFD[0], &val2, sizeof(int)) == -1) {
            printf("Child read error\n");
            return 0;
        }

        val2 = val2 * val2;

        //wyslij potokiem nienazwanym val2 do procesu macierzysego
        close(toParentFD[0]);
        if (write(toParentFD[1], &val2, sizeof(int)) == -1) {
            printf("Child write error\n");
            return 0;
        }

        close(toChildFD[0]);
        close(toParentFD[1]);

    } else {

        val1 = atoi(argv[1]);

        //wyslij val1 potokiem nienazwanym do procesu potomnego
        close(toChildFD[0]);
        if (write(toChildFD[1], &val1, sizeof(int)) == -1) {
            printf("Write parent error\n");
            return 0;
        }

        sleep(1);

        //odczytaj z potoku nienazwanego wartosc przekazana przez proces potomny i zapisz w zmiennej val3
        close(toChildFD[1]);
        if (read(toParentFD[0], &val3, sizeof(int)) == -1) {
            printf("Read parent error\n");
            return 0;
        }

        printf("%d square is: %d\n", val1, val3);


        close(toChildFD[0]);
        close(toParentFD[1]);
    }

    return 0;
}
