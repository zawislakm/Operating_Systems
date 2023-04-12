#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

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


double f(double x) {
    double value = 4 / (x * x + 1);
    return value;
}

double sumUp(double s, double e, double ac) {
    double value = 0.0;
    for (double i = s; i < e; i += ac) {
        value += f(i) * ac;
    }
    return value;
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        exit(1);
    }
    start_clock();
    double ac = atof(argv[1]);
    int n = atoi(argv[2]);

    int pipes[n + 1];
    int fork_id;

    for (int i = 1; i < n + 1; i++) {
        int fd[2];
        int pipe_id = pipe(fd);
        if (pipe_id == -1) {
            printf("Pipe error");
            exit(1);
        }

        fork_id = fork();
        if (fork_id == -1) {
            printf("Fork error");
            exit(1);
        }

        if (fork_id == 0) {
            close(fd[0]);

            double s = 1.0 / (double) n * (double) (i - 1);
            double e = 1.0 / (double) n * (double) i;


            double value = sumUp(s, e, ac);
            write(fd[1], &value, sizeof(double));
            close(fd[1]);
            return 0;

        } else {
            close(fd[1]);
            pipes[i - 1] = fd[0];
        }
    }

    if (fork_id != 0) {
        double ans = 0.0;
        for (int i = 0; i < n; i++) {
            double tmp = 0.0;
            read(pipes[i], &tmp, sizeof(double));
            ans += tmp;
            close(pipes[i]);
        }

        end_clock();
        printf("Obliczona wartosc: %.10lf\n", ans);
        return 0;
    }
}
