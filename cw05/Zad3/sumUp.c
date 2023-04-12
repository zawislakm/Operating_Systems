#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

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

    double ac = atof(argv[1]);
    double s = atof(argv[2]);
    double e = atof(argv[3]);

    double value = sumUp(s, e, ac);
    char *path = "/tmp/queue";

    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        printf("Open error");
        exit(1);
    }

    if (write(fd, &value, sizeof(value)) == -1) {
        printf("Write error");
        exit(1);
    }

    close(fd);

    return 0;
}
