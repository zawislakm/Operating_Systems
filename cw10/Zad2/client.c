
#include "Repsonds.h"

int unix_work(char *unix_path,char* user) {
    struct sockaddr_un soc_add,bind_add;
    memset(&soc_add, 0, sizeof(soc_add));
    bind_add.sun_family = AF_UNIX;
    soc_add.sun_family = AF_UNIX;
    snprintf(bind_add.sun_path,sizeof bind_add.sun_path,"/tmp/%s%ld",user,time(NULL));
    strncpy(soc_add.sun_path, unix_path, sizeof soc_add.sun_path);

    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    bind(sock,(void *) &bind_add, sizeof soc_add);
    connect(sock, (struct sockaddr *) &soc_add, sizeof soc_add);

    return sock;
}

int web_work(char *ip, int port) {
    struct sockaddr_in soc_add;
    memset(&soc_add, 0, sizeof(soc_add));
    soc_add.sin_family = AF_INET;
    soc_add.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &soc_add.sin_addr) <= 0) {
        printf("Wrong addres\n");
        exit(0);
    }
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    connect(sock, (struct sockaddr *) &soc_add, sizeof soc_add);
    return sock;
}


int sock;

void handler(int signo) {
    message to_send = {.value=DISCONNECT};
    sendto(sock,&to_send,sizeof to_send,0,NULL,sizeof (struct sockaddr_in));
    exit(0);
}

void extractLastWord(char array[], char lastWord[]) {
    int length = strlen(array);
    int lastIndex = -1;

    // Skip leading spaces
    int startIndex = 0;
    while (array[startIndex] == ' ') {
        startIndex++;
    }

    for (int i = length - 1; i >= startIndex; i--) {
        if (array[i] == ' ') {
            lastIndex = i;
            break;
        }
    }

    if (lastIndex != -1) {
        int lastWordLength = length - lastIndex;
        strncpy(lastWord, array + lastIndex + 1, lastWordLength);
        lastWord[lastWordLength] = '\0';
        array[lastIndex] = '\0';
    } else {
        lastWord[0] = '\0';
    }
}


int main(int argc, char **argv) {

    if (strcmp(argv[2], "web") == 0 && argc == 5) {
        sock = web_work(argv[3], atoi(argv[4]));
    }

    if (strcmp(argv[2], "unix") == 0 && argc == 4) {
        sock = unix_work(argv[3],argv[1]);
    }


    signal(SIGINT, handler);

    char *nickname = argv[1];
    message to_send = {.value = CONNECT};
    strncpy(to_send.other_nickname,nickname,sizeof to_send.other_nickname);
    send(sock,&to_send,sizeof to_send,0);


    int epoll_fd = epoll_create1(0);

    struct epoll_event stdin_event = {
            .events = EPOLLIN | EPOLLPRI,
            .data = {.fd = STDIN_FILENO}
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);

    struct epoll_event server_event = {
            .events = EPOLLIN | EPOLLPRI | EPOLLHUP,
            .data = {.fd = sock}
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &server_event);

    struct epoll_event events[2];
    while (1) {

        int actions = epoll_wait(epoll_fd, events, 2, 1);

        for (int i = 0; i < actions; i++) {
            if (events[i].data.fd == STDIN_FILENO) {

                char buffer[512] = {};
                int x = read(STDIN_FILENO, &buffer, 512);
                buffer[x] = 0;
                char command[100];
                char second[100];
                sscanf(buffer, "%99s %[^,]", command, second);
                bool right = true;

                messages_value type = -1;
                char other_nickname[MAX_LEN] = {};
                char text[MAX_LEN] = {};

                if (strcmp(command, "LIST") == 0) {
                    type = LIST;
                } else if (strcmp(command, "2ALL") == 0) {
                    type = ALL;
                    memcpy(text, second, strlen(second) * sizeof(char));
                    text[strlen(second)] = 0;
                    other_nickname[0] = 0;
                } else if (strcmp(command, "2ONE") == 0) {
                    type = ONE;
                    char lastWord[100];
                    extractLastWord(second, lastWord);
                    memcpy(text, second, strlen(second) * sizeof(char));
                    text[strlen(command)] = 0;
                    memcpy(other_nickname, lastWord, strlen(lastWord) * sizeof(char));
                    other_nickname[strlen(lastWord)] = 0;
                } else if (strcmp(command, "STOP") == 0) {
                    type = STOP;
                } else {
                    right = false;
                    printf("Wrong command\n");
                }

                if (right) {
//                    printf("Other_nickname: %s\n", other_nickname);
//                    printf("Text:%s\n", text);
                    message msg;
                    msg.value = type;
                    memcpy(&msg.other_nickname, other_nickname, strlen(other_nickname) + 1);
                    memcpy(&msg.text, text, strlen(text) + 1);

                    sendto(sock,&msg,sizeof msg,0,NULL,sizeof (struct sockaddr_in));
                }
            } else {

                message to_read;
                recvfrom(sock,&to_read,sizeof to_read,0,NULL,NULL);
                switch (to_read.value) {
                    case TAKEN:
                        printf("This nick is already TAKEN\n");
                        close(sock);
                        exit(0);
                        break;
                    case FULL:
                        printf("Server is full\n");
                        close(sock);
                        exit(0);
                        break;
                    case PING:
                        int result = write(sock, &to_read, sizeof to_read);
                        break;
                    case STOP:
                        printf("Stopping\n");
                        close(sock);
                        exit(0);
                        break;
                    case RESPONSE:
                        printf("%s\n", to_read.text);
                        break;
                    default:
                        printf("Unknown value\n");
                        break;
                }

                if (events[i].events & EPOLLHUP) {
                    printf("Disconnected\n");
                    exit(0);
                }
            }
        }
    }

    return 0;
}