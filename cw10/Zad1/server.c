#include "Repsonds.h"
#include <pthread.h>

#define MAX_USERS 3
#define PING_FREQUENCY 15

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;


struct client {
    int fd;
    enum client_state state;
    char nickname[20];
    bool responding;
} clients[MAX_USERS];
typedef struct client client;

typedef struct event_data {
    enum event_type type;
    union payload {
        client *client;
        int socket;
    } payload;
} event_data;


void delete_client(client *client) {
    client->state = empty;
    printf("[%s-Server] Deleting client: %s\n",getTime(), client->nickname);
    client->nickname[0] = 0;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);
}

void send_message(client *client, messages_value value, char text[MAX_LEN]) {
    message to_send;
    to_send.value = value;
    memcpy(&to_send.text, text, MAX_LEN * sizeof(char));
    int result = write(client->fd, &to_send, sizeof(to_send));
}

int check_name(client *client, int j) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (i != j && strcmp(clients[i].nickname, client->nickname) == 0) {
            return -1;
        }
    }

    return 1; // The nickname doesn't exist
}


void recived_message(client *client) {
    if (client->state == init) {
        int nick_size = read(client->fd, client->nickname, sizeof client->nickname - 1);
        int j = client - clients;
        pthread_mutex_lock(&mutex);
        if (check_name(client, j) == 1) {
            client->nickname[nick_size] = '\0';
            client->state = ready;
            printf("[%s-Server] New client: %s\n",getTime(), client->nickname);
        } else {
            message to_send = {.value=TAKEN};
            printf("[%s-Server] Nickname: %s already taken\n",getTime(), client->nickname);
            int result = write(client->fd, &to_send, sizeof to_send);
            strcpy(client->nickname, "new client");
            delete_client(client);
        }
        pthread_mutex_unlock(&mutex);
    }else{

        message to_read;
        int result = read(client->fd,&to_read,sizeof to_read);

        printf("[%s-Server] Got message type: %s, message: %s\n",getTime(), enumNames[to_read.value],to_read.text);

        switch (to_read.value) {
            case PING:
                pthread_mutex_lock(&mutex);
                printf("[%s-Server] Ping %s\n",getTime(), client->nickname);
                client->responding = true;
                pthread_mutex_unlock(&mutex);
                break;
            case DISCONNECT:
            case STOP:
                pthread_mutex_lock(&mutex);
                delete_client(client);
                pthread_mutex_unlock(&mutex);
                break;
            case ALL: {
                char text[MAX_LEN] = "";
                strcat(text, client->nickname);
                strcat(text, ": ");
                strcat(text, to_read.text);

                for (int i = 0; i < MAX_USERS; i++) {
                    if (clients[i].state != empty) {
                        send_message(clients + i, RESPONSE, text);
                    }
                }
                break;
            }
            case LIST:
                for (int i = 0; i < MAX_USERS; i++) {
                    if (clients[i].state != empty) {
                        send_message(client, RESPONSE, clients[i].nickname);
                    }
                }
                break;
            case ONE: {
                char text[MAX_LEN] = "";
                strcat(text, client->nickname);
                strcat(text, ": ");
                strcat(text, to_read.text);
                for (int i = 0; i < MAX_USERS; i++) {
                    if (clients[i].state != empty) {
                        if (strcmp(clients[i].nickname, to_read.other_nickname) == 0) {
                            send_message(clients + i, RESPONSE, text);
                        }
                    }
                }
                break;
            }
            default:
                printf("Unknown value\n");
                break;
        }

    }
}


void init_socket(int socket,void * addr,int addr_size){
    bind(socket,(struct sockaddr*) addr, addr_size);
    listen(socket,MAX_USERS);
    struct epoll_event event = {.events = EPOLLIN | EPOLLPRI};
    event_data* event_data = event.data.ptr = malloc(sizeof *event_data);
    event_data->type = socket_event;
    event_data->payload.socket = socket;
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,socket,&event);
}

int find_free(){
    for(int i =0;i<MAX_USERS;i++){
        if(clients[i].state == empty){
            return i;
        }
    }
    return -1;
}

client* new_client(int client_fd){
    pthread_mutex_lock(&mutex);
    int i = find_free();
    if (i ==-1){
        return NULL;
    }
    client* new_client = &clients[i];

    new_client->fd = client_fd;
    new_client->state = init;
    new_client->responding = true;
    pthread_mutex_unlock(&mutex);

    return new_client;

}

void* ping (void* _){
    static message to_send = {.value=PING};
//    printf("PIGNGS started\n");
    while (1){
        sleep(PING_FREQUENCY);
        pthread_mutex_lock(&mutex);
        printf("[%s-Server] Pinging clients\n",getTime());
        for (int i =0 ;i < MAX_USERS;i++){
            if(clients[i].state != empty){
                if(clients[i].responding){
                    clients[i].responding = false;
                    int result = write(clients[i].fd,&to_send,sizeof to_send);
                } else{
                    delete_client(&clients[i]);
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char** argv){
    if (argc != 3){
        printf("Wrong armunets");
        exit(0);
    }
    int port = atoi(argv[1]);
    char* socket_path = argv[2];

    epoll_fd = epoll_create1(0);

    struct sockaddr_un local_addr = {.sun_family = AF_UNIX};
    strncpy(local_addr.sun_path, socket_path,sizeof local_addr.sun_path);

    struct sockaddr_in web_addr ={
            .sin_family = AF_INET,.sin_port = htons(port),
            .sin_addr = {.s_addr = htonl(INADDR_ANY)}
    };

    unlink(socket_path);
    int local_socket = socket(AF_UNIX,SOCK_STREAM,0);
    init_socket(local_socket,&local_addr,sizeof local_addr);

    int web_socket = socket(AF_INET,SOCK_STREAM,0);
    init_socket(web_socket,&web_addr,sizeof web_addr);

    pthread_t ping_thread;
    pthread_create(&ping_thread,NULL,ping,NULL);

    printf("[%s-Server], Server stared on: %d and path: %s\n",getTime(),port,socket_path);

    struct epoll_event events[20];

    while (1){
        int actions = epoll_wait(epoll_fd,events,10,-1);
        for(int i =0; i<actions;i++){
            event_data* data = events[i].data.ptr;

            if(data->type == socket_event){ //new connection
                int client_fd = accept(data->payload.socket,NULL,NULL);
                client* client = new_client(client_fd);

                if (client == NULL){
                    printf("[%s-Server] Server is full\n",getTime());
                    message to_send = { .value = FULL };
                    int result = write(client_fd,&to_send,sizeof (to_send));
                    close(client_fd);
                    continue;
                }
                event_data* event_data = malloc(sizeof (event_data));
                event_data->type = client_event;
                event_data->payload.client = client;
                struct epoll_event event = {.events = EPOLLIN | EPOLLET | EPOLLHUP, .data={event_data}};

                epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_fd,&event);

            } else if (data->type == client_event){
                if (events[i].events & EPOLLHUP){
                    pthread_mutex_lock(&mutex);
                    delete_client(data->payload.client);
                    pthread_mutex_unlock(&mutex);
                }else{
                    recived_message(data->payload.client);
                }
            }
        }
    }

    return 0;
}


















