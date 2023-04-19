#include "Repsonds.h"
#define MAXCLIENT 3
//link list
typedef struct server_user_list{
    int clientID;
    int queueID;
    struct server_user_list * next;
}serverUsers;


int serverInsert(serverUsers ** list, int ID, int queueID){
    int count = 0;
    serverUsers * current = *list;
    while (current != NULL){
        count+=1;
        current = current-> next;
    }
    if (count == MAXCLIENT){
        return -1;
    }
    serverUsers * nl = calloc(1, sizeof(serverUsers));
    nl->next = *list;
    *list = nl;
    nl->queueID = queueID;
    nl->clientID = ID;
    return 1;

}
void serverFree(serverUsers * list){
    if(list != NULL){
        serverUsers * next = list->next;
        free(list);
        serverFree(next);
    }
}

void serverRemove(serverUsers ** list, int ID){
    serverUsers * now = *list;
    serverUsers * prev = NULL;
    while(now != NULL){
        if (now->clientID == ID){
            if(now == *list){
                *list = now->next;
                free(now);
            }
            else{
                prev->next = now->next;
                free(now);
            }
            return;
        }
        prev = now;
        now = now->next;
    }
}

serverUsers * serverFind(serverUsers * list, int ID){
    if(list == NULL)
        return list;
    if(ID == list->clientID)
        return list;
    return serverFind(list->next, ID);
}


