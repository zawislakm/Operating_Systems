#ifndef LAB7_COMMON_H
#define LAB7_COMMON_H
#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>


#define CHAIRS 2
#define BARBERS 3
#define CLIENTS 6
#define WAITING_ROOM 2
#define HAIRCUTS 5

#define ID_PROJECT getenv("HOME")
#define SEM_Q "0"
#define SEM_C "1"
#define SEM_B "2"
#define SEM_BM "3"

typedef struct {
    int toTake;
    int toPut;
    char queue[WAITING_ROOM];
}SharedMemory;

time_t timestamp;
struct tm *local_time;


char *getTime() {
    char *time_str = (char *) malloc(sizeof(char) * 9);

    time(&timestamp);
    local_time = localtime(&timestamp);
    strftime(time_str, 9, "%H:%M:%S", local_time);
    return time_str;
}



#endif //LAB7_COMMON_H


