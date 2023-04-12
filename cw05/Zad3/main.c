#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// time
struct timespec start_realtime;
struct timespec end_realtime;


void start_clock() {
    clock_gettime(CLOCK_REALTIME, &start_realtime);
}

void end_clock() {
    clock_gettime(CLOCK_REALTIME, &end_realtime);

    long elapsed_realtime_ns = (end_realtime.tv_sec - start_realtime.tv_sec) * 1000000000 +
                               (end_realtime.tv_nsec - start_realtime.tv_nsec);

    printf("\n");
    printf("Czas rzeczywisty: %ld ns\n", elapsed_realtime_ns);

}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        exit(1);
    }
    start_clock();

    char *path = "/tmp/queue";

    mkfifo(path, 0666);


    int n = atoi(argv[2]);


    int fork_id;

    for (int i = 1; i < n + 1; i++) {

        fork_id = fork();
        if (fork_id == -1) {
            printf("Fork error");
            exit(1);
        }

        if (fork_id == 0) {

            double s = 1.0 / (double) n * (double) (i - 1);
            double e = 1.0 / (double) n * (double) i;
            char arg1[20], arg2[20];
            sprintf(arg1, "%.5f", s);
            sprintf(arg2, "%.5f", e);


            execl("./sumUp", "sumUp", argv[1], arg1, arg2, NULL);
            return 0;
        }
    }

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("Open error");
        exit(1);
    }

    double ans = 0.0;
    for (int i = 0; i < n; i++) {
        double tmp = 0.0;
        read(fd, &tmp, sizeof(double));
        ans += tmp;

    }

    end_clock();
    printf("Obliczona wartosc: %.10lf\n", ans);
    return 0;

}
