#include "Repsonds.h"
#include <pthread.h>

#define MAX_USERS 3
#define PING_FREQUENCY 15

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;

union addr {
    struct sockaddr_un uni;
    struct sockaddr_in web;
};

typedef struct sockaddr *server_sockaddr;

struct client {
    union addr addr;
    int sock, addrlen;
    enum client_state state;
    char nickname[20];
    bool responding;
} clients[MAX_USERS];
typedef struct client client;



void delete_client(client *client) {
    printf("[%s-Server] Deleting client: %s\n", getTime(), client->nickname);
    message to_send = {.value=DISCONNECT};
    sendto(client->sock, &to_send, sizeof to_send, 0, (server_sockaddr) &client->addr, client->addrlen);
    memset(&client->addr,0,sizeof client->addr);
    client->state = empty;
    client->nickname[0] = 0;
    client->sock = 0;

}

void send_msg(client *client, messages_value value, char text[MAX_LEN]) {
    message to_send;
    to_send.value = value;
    memcpy(&to_send.text, text, MAX_LEN * sizeof(char));
    sendto(client->sock, &to_send, sizeof to_send, 0, (server_sockaddr) &client->addr, client->addrlen);
}


void recived_message(client *client, message *to_read_ptr) {

    message to_read = *to_read_ptr;

    printf("[%s-Server] Got message type: %s, message: %s\n", getTime(), enumNames[to_read.value], to_read.text);

    switch (to_read.value) {
        case PING:
            pthread_mutex_lock(&mutex);
            printf("[%s-Server] Ping %s\n", getTime(), client->nickname);
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
                    send_msg(clients + i, RESPONSE, text);
                }
            }
            break;
        }
        case LIST:
            for (int i = 0; i < MAX_USERS; i++) {
                if (clients[i].state != empty) {
                    send_msg(client, RESPONSE, clients[i].nickname);
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
                        send_msg(clients + i, RESPONSE, text);
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




void init_socket(int socket, void *addr, int addr_size) {
    bind(socket, (struct sockaddr *) addr, addr_size);
    struct epoll_event event = {
            .events = EPOLLIN | EPOLLPRI,
            .data = {.fd = socket}
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

int check_name(char *nickname) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (strcmp(clients[i].nickname, nickname) == 0) {
            return -1;
        }
    }

    return 1; // The nickname doesn't exist
}

int find_free() {
    for (int i = 0; i < MAX_USERS; i++) {
        if (clients[i].state == empty) {
            return i;
        }
    }
    return -1;
}

void new_client(union addr *addr, socklen_t addrlen, int sock, char *nickname) {
    pthread_mutex_lock(&mutex);
    int empty_index = find_free();

    if (empty_index == -1) {
        pthread_mutex_unlock(&mutex);
        printf("[%s-Server] Server full\n", getTime());
        message to_send = {.value = FULL};
        sendto(sock, &to_send, sizeof to_send, 0, (server_sockaddr) addr, addrlen);
        return ;
    }

    if (check_name(nickname) == -1) {
        pthread_mutex_unlock(&mutex);
        printf("[%s-Server] Nickname is already taken\n", getTime());
        message to_send = {.value = TAKEN};
        sendto(sock, &to_send, sizeof to_send, 0, (server_sockaddr) addr, addrlen);
        return;
    }

    printf("[%s-Server] New client %s\n", getTime(), nickname);

    client *new_client = &clients[empty_index];
    memcpy(&new_client->addr, addr, addrlen);
    new_client->addrlen = addrlen;
    new_client->state = ready;
    new_client->responding = true;
    new_client->sock = sock;

    memset(new_client->nickname, 0, sizeof new_client->nickname);
    strncpy(new_client->nickname, nickname, sizeof new_client->nickname - 1);


    pthread_mutex_unlock(&mutex);

}

void *ping(void *_) {
    static message to_send = {.value=PING};

    while (1) {
        sleep(PING_FREQUENCY);
        pthread_mutex_lock(&mutex);
        printf("[%s-Server] Pinging clients\n", getTime());
        for (int i = 0; i < MAX_USERS; i++) {
            if (clients[i].state != empty) {
                if (clients[i].responding) {
                    clients[i].responding = false;
                    sendto(clients[i].sock, &to_send, sizeof to_send, 0, (server_sockaddr) &clients[i].addr, clients[i].addrlen);
                } else {
                    delete_client(&clients[i]);
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Wrong armunets");
        exit(0);
    }
    int port = atoi(argv[1]);
    char *socket_path = argv[2];

    epoll_fd = epoll_create1(0);

    struct sockaddr_un local_addr = {.sun_family = AF_UNIX};
    strncpy(local_addr.sun_path, socket_path, sizeof local_addr.sun_path);

    struct sockaddr_in web_addr = {
            .sin_family = AF_INET, .sin_port = htons(port),
            .sin_addr = {.s_addr = htonl(INADDR_ANY)}
    };

    unlink(socket_path);
    int local_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    init_socket(local_socket, &local_addr, sizeof local_addr);

    int web_socket = socket(AF_INET, SOCK_DGRAM, 0);
    init_socket(web_socket, &web_addr, sizeof web_addr);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);

    printf("[%s-Server], Server stared on: %d and path: %s\n", getTime(), port, socket_path);

    struct epoll_event events[20];

    while (1) {
        int actions = epoll_wait(epoll_fd, events, 10, 0);
        for (int i = 0; i < actions; i++) {
            int socket = events[i].data.fd;
            message to_read;
            union addr addr;
            socklen_t addrlen = sizeof addr;
            recvfrom(socket, &to_read, sizeof to_read, 0, (server_sockaddr) &addr, &addrlen);

            if (to_read.value == CONNECT) {
                new_client(&addr, addrlen, socket, to_read.other_nickname);
            } else {
                for (int i = 0; i < MAX_USERS; i++) {
                    if (clients[i].state != empty) {
                        if (memcmp( &clients[i].addr, &addr, addrlen) == 0){
                            recived_message(&clients[i], &to_read);
                        }
                    }
                }
            }


        }
    }

    return 0;
}


















