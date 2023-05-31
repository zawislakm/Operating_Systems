#ifndef DEFINES_H
#define DEFINES_H

#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEN 256

typedef enum {
    STOP,
    LIST,
    INIT,
    RESPONSE,
    ALL,
    ONE,
    SHOWN,
    DISCONNECT,
    TAKEN,
    FULL,
    PING,
} messages_value;

const char *enumNames[] = {
        "STOP",
        "LIST",
        "INIT",
        "RESPONSE",
        "2ALL",
        "2ONE",
        "SHOWN",
        "DISCONNECT",
        "TAKEN",
        "FULL",
        "PING"
};

enum client_state {
    empty = 0,ready,init
};
enum event_type {
    socket_event, client_event
};


typedef struct {
    messages_value value;
    char text[MAX_LEN];
    char other_nickname[MAX_LEN];
}message;


time_t timestamp;
struct tm *local_time;
char *getTime() {
    char *time_str = (char *) malloc(sizeof(char) * 9);

    time(&timestamp);
    local_time = localtime(&timestamp);
    strftime(time_str, 9, "%H:%M:%S", local_time);
    return time_str;
}

#endif