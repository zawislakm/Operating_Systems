#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>
#include "serverList.h"

int work = 1;

struct {
    long flag;
    char message[MSGMAX];
} receiver;

struct {
    long flag;
    char message[MSGMAX];
}sender;


void log_server() {
    FILE *file = fopen("server.txt", "a");
    if (file == NULL) {
        printf("Something went wrong during logging server\n");
        exit(1);
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (receiver.flag == STOP) {
        fprintf(file, "Server shut down [%02d:%02d:%02d]\n", t->tm_hour, t->tm_min, t->tm_sec);

    } else {
        int clientID;
        char read[MSGMAX];
        sscanf(receiver.message, "%d,%[^,]", &clientID, read);
        fprintf(file, "[%02d:%02d:%02d], from %d,type: %s, message: %s\n", t->tm_hour, t->tm_min, t->tm_sec, clientID,
                enumNames[receiver.flag], read);
    }
    fclose(file);

}


void exitHandler(int signo) {
    log_server();
    work = 0;
}

void serverINIT(int *nextClientID, serverUsers **users) {
    int clientQ;
    sscanf(receiver.message, "%d", &clientQ);
    int output = serverInsert(users, *nextClientID, clientQ);

    memset(sender.message,0,sizeof(sender.message));

    if (output == -1) {
        sprintf(sender.message, "%d\n", -1);
        printf("Out to place for users\n");
    } else {
        sprintf(sender.message, "%d\n", *nextClientID);
        printf("Respond init, Receive queue id %d\n", clientQ);
    }
    sender.flag = RESPONSE;
    msgsnd(clientQ, &sender, strlen(sender.message) + 1, IPC_NOWAIT);

    (*nextClientID)++;
}

void serverONE(serverUsers *users, int clientID, int clientTo, char *massage) {
    serverUsers *clientData = serverFind(users, clientTo);
    if (!clientData)
        return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char str[MSGMAX];
    sprintf(str, "[%02d:%02d:%02d], sent by: %d, massage: %s", t->tm_hour, t->tm_min, t->tm_sec, clientID, massage);

    memset(sender.message,0,sizeof(sender.message));
    sender.flag = RESPONSE;
    sprintf(sender.message, "%s", str);
    msgsnd(clientData->queueID, &sender, MSGMAX, IPC_NOWAIT);
}

void serverALL(serverUsers *clienets, int clientID, char *massage) {
    serverUsers *clientData = serverFind(clienets, clientID);
    if (!clientData)
        return;

    memset(sender.message,0,sizeof(sender.message));
    sender.flag = RESPONSE;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char str[MSGMAX];
    sprintf(str, "[%02d:%02d:%02d], sent by: %d, massage: %s", t->tm_hour, t->tm_min, t->tm_sec, clientID, massage);
    sprintf(sender.message, "%s", str);

    while (clienets != NULL) {
        if (clienets->clientID != clientID) {
            msgsnd(clienets->queueID, &sender, MSGMAX, IPC_NOWAIT);
        }
        clienets = clienets->next;
    }
}


void serverLIST(serverUsers *users) {
    int clientID;
    sscanf(receiver.message, "%d", &clientID);
    serverUsers *clientData = serverFind(users, clientID);
    if (!clientData)
        return;
    memset(sender.message,0,sizeof(sender.message));
    sender.flag = RESPONSE;
    while (users != NULL) {
        sprintf(sender.message + strlen(sender.message), "Client number: %d\n", users->clientID);
        users = users->next;
    }
    msgsnd(clientData->queueID, &sender, MSGMAX, IPC_NOWAIT);
    memset(sender.message, 0, sizeof(sender.message));
}


void serverSTOP(serverUsers **users) {
    int clId;
    sscanf(receiver.message, "%d", &clId);
    serverRemove(users, clId);
}

int main() {
    //Server log create
    FILE *file = fopen("server.txt", "w");
    if (file == NULL) {
        printf("Something went wrong during logging server\n");
        exit(1);
    }
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char date[20];
    strftime(date, 20, "%Y-%m-%d %H:%M:%S", local);
    fprintf(file, "Date: %s\n", date);
    fclose(file);

    //queue open
    key_t ipcKey = SERVER;
    msgctl(ipcKey, IPC_RMID, NULL);
    int msgDesc = msgget(ipcKey, IPC_CREAT | IPC_EXCL | 0666);

    if (msgDesc == -1) {
        printf("Something wrong during open\n");
        exit(0);

    }
    int msgKey = msgDesc;

    signal(SIGINT, exitHandler);
    printf("Server started!\n");

    int firstFreeID = 0;
    serverUsers *users = NULL;

    while (work) {
        printf("Waiting for commands\n");
        size_t nRead = msgrcv(msgKey, &receiver, sizeof(receiver) - sizeof(long), -100, MSG_NOERROR);
        printf("Command received\n");
        log_server();
        switch (receiver.flag) {
            case STOP:
                printf("Stop work!\n");
                serverSTOP(&users);
                break;
            case INIT:
                printf("Init work!\n");
                serverINIT(&firstFreeID, &users);
                break;
            case LIST:
                printf("List work!\n");
                serverLIST(users);
                break;
            case ALL:
                printf("2ALL work!\n");
                int clientID;
                char readALL[MSGMAX];
                sscanf(receiver.message, "%d,%[^,]", &clientID, readALL);
                serverALL(users, clientID, readALL);
                break;
            case ONE:
                printf("2ONE work!\n");
                int clientID1;
                char idChar;
                char readONE[MSGMAX];
                sscanf(receiver.message, "%d,%c %[^,]", &clientID1, &idChar, readONE);
                int clinetTo = atoi(&idChar);
                serverONE(users, clientID1, clinetTo, readONE);
                break;
        }
    }
    printf("CtrlC\n");
    serverUsers *client = users;
    while (client != NULL) {
        receiver.flag = STOP;
        msgsnd(client->queueID, &receiver, MSGMAX, IPC_NOWAIT);
        client = client->next;
    }

    serverFree(users);

    msgctl(msgKey, IPC_RMID, NULL);

}