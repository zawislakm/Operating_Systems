#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>
#include <time.h>
#include "Repsonds.h"

struct {
    long flag;
    char data[MSGMAX];
} request;


int sQueue;
int clientID;
int clientKey;
int work =1;

void handler(int signo) {

    printf("Quit\n");
    request.flag = STOP;
    sprintf(request.data, "%d", clientID);
    msgsnd(sQueue, &request, MSGMAX, 0);
    msgctl(clientKey, IPC_RMID, 0);
    work = 0;
    exit(0);
}

void incoming_mes() {

    if ((msgrcv(clientKey, &request, MSGMAX, RESPONSE, IPC_NOWAIT) >= 0) ||(msgrcv(clientKey, &request, MSGMAX, STOP, IPC_NOWAIT) >= 0) ) {
        if (request.flag == STOP){
            printf("Server dead\n");
            work = 0;
        }
        else if (request.flag == RESPONSE) {
            printf("Received message: %s\n", request.data);
        }
    }
    request.flag = SHOWN;

}

int main() {

    signal(SIGINT, handler);

    srand(time(NULL));
    printf("Client create!\n");
    sQueue = msgget(SERVER, 0);
    if (sQueue == -1) {
        puts("Queue server not working\n");
        return 0;
    }
    struct passwd *path = getpwuid(getuid()); //DIR path
    key_t qKey = ftok(path->pw_dir, rand() % 256);
    clientKey = msgget(qKey, IPC_CREAT | IPC_EXCL | 0666);

    if (clientKey == -1) {
        puts("Unable to open queue. Try again!\n");
        return 0;
    }
    printf("Client key: %d.\n", clientKey);

    request.flag = INIT;
    sprintf(request.data, "%d", clientKey);
    msgsnd(sQueue, &request, MSGMAX, IPC_NOWAIT);

    printf("Waiting for server\n");
    msgrcv(clientKey, &request, MSGMAX, RESPONSE, 0);
    sscanf(request.data, "%d", &clientID);
    if (clientID == -1){
        printf("No place for client in server, try again later!\n");
        return 0;
    }
    printf("Server answered. ID: %d\n", clientID);


    char *input = NULL;
    char command[100];
    char second[100];
    size_t nRead = 0;


    while (work != 0) {
        getline(&input, &nRead, stdin);
        incoming_mes();
        sscanf(input, "%99s %[^,]", command, second);

        if (strcmp(command, "LIST") == 0) {
            request.flag = LIST;
            sprintf(request.data, "%d", clientID);
            msgsnd(sQueue, &request, MSGMAX, 0);
            msgrcv(clientKey, &request, MSGMAX, RESPONSE, 0);
            printf("%s\n", request.data);
            request.flag = SHOWN;

        }
        if (strcmp(command, "2ALL") == 0) {
            request.flag = ALL;
            sprintf(request.data, "%d,%s", clientID, second);
            msgsnd(sQueue, &request, MSGMAX, 0);
            request.flag=SHOWN;
        }
        if (strcmp(command, "2ONE") == 0) {
            request.flag = ONE;
            sprintf(request.data, "%d,%s", clientID, second);
            msgsnd(sQueue, &request, MSGMAX, 0);
            request.flag=SHOWN;
        }

        if (strcmp(command, "STOP") == 0) {
            handler(SIGINT);
        }

        sscanf("PLaceHolder","%99s",command);
    }
    msgctl(clientKey, IPC_RMID, 0);
    free(input);
    return 0;
}