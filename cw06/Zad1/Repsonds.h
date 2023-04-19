#ifndef DEFINES_H
#define DEFINES_H
#define MSGMAX 4096

#include <stdlib.h>
#include <string.h>

#define SERVER 664207


typedef enum {
    STOP = 1,
    LIST,
    INIT,
    RESPONSE,
    ALL,
    ONE,
    SHOWN,
} messages;

const char *enumNames[] = {
        "STOP",
        "LIST",
        "INIT",
        "RESPONSE",
        "2ALL",
        "2ONE",
        "SHOWN",
};

#endif