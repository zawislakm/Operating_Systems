#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>


#define REINDEER_AMOUNT 9
#define  ELVES_AMOUNT 10
#define ELVES_QUEUE_LENGTH 3

time_t timestamp;
struct tm *local_time;
char *getTime() {
    char *time_str = (char *) malloc(sizeof(char) * 9);

    time(&timestamp);
    local_time = localtime(&timestamp);
    strftime(time_str, 9, "%H:%M:%S", local_time);
    return time_str;
}

int reindeer_comeback = 0;
int end_christmas_value = 0;
bool reindeer_waiting = false;
int elves_in_queue = 0;
int elves_queue[ELVES_QUEUE_LENGTH];

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_mutex_wait = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_cond_wait = PTHREAD_COND_INITIALIZER;
pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_mutex_wait = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_cond_wait = PTHREAD_COND_INITIALIZER;


void clear_queue(int *q) {
    for (int i = 0; i < ELVES_QUEUE_LENGTH; i++) {
        q[i] = -1;
    }
}


void *elf_loop(void *arg) {
    int* id_pointer = (int*)arg;
    int ID = *id_pointer;
    free(id_pointer);

    while (true) {
        int time_sleep = rand() % 5 + 2;
        sleep(time_sleep);

        pthread_mutex_lock(&elf_mutex_wait);

        if (elves_in_queue == ELVES_QUEUE_LENGTH) {
            printf("[%s-Elf-%d] Samodzielnie rozwiazuje swoj problem\n",getTime(), ID);
        }
        pthread_mutex_unlock(&elf_mutex_wait);

        pthread_mutex_lock(&elf_mutex);

        if (elves_in_queue < ELVES_QUEUE_LENGTH) {
            elves_queue[elves_in_queue] = ID;
            elves_in_queue += 1;
            printf("[%s-Elf-%d] Aktualnie %d elfow czeka na Mikolaja\n",getTime(), ID, elves_in_queue);

            if (elves_in_queue == ELVES_QUEUE_LENGTH) {
                printf("[%s-Elf-%d] Wybudzam Mikolaja\n",getTime(), ID);
                pthread_mutex_lock(&santa_mutex);
                pthread_cond_broadcast(&santa_cond);
                pthread_mutex_unlock(&santa_mutex);
            }
        }

        pthread_mutex_unlock(&elf_mutex);
    }

}


void *reindeer_loop(void *arg) {
    int* id_pointer = (int*)arg;
    int ID = *id_pointer;
    free(id_pointer);

    while (true) {

        pthread_mutex_lock(&reindeer_mutex_wait);
        while (reindeer_waiting == true) {
            pthread_cond_wait(&reindeer_cond_wait, &reindeer_mutex_wait);
        }
        pthread_mutex_unlock(&reindeer_mutex_wait);

        int time_sleep = rand() % 10 + 5;
        sleep(time_sleep);

        pthread_mutex_lock(&reindeer_mutex);
        reindeer_comeback += 1;
        printf("[%s-Renifer-%d] Aktualnie %d reniferow czeka na Mikolaja\n", getTime(),ID, reindeer_comeback);
        reindeer_waiting = true;

        if (reindeer_comeback ==REINDEER_AMOUNT){
            printf("[%s-Renifer-%d] Wybudzam Mikolaja\n",getTime(),ID);
            pthread_mutex_lock(&santa_mutex);
            pthread_cond_broadcast(&santa_cond);
            pthread_mutex_unlock(&santa_mutex);
        }
        pthread_mutex_unlock(&reindeer_mutex);

    }

}

int main() {

    srand(time(NULL));
    printf("Simulation start: %s\n",getTime());


    pthread_t reindeer_t[REINDEER_AMOUNT];
    for (int i = 0; i < REINDEER_AMOUNT; i++){
        int* id_pointer = malloc(sizeof (int));
        *id_pointer = i;
        pthread_create(&reindeer_t[i], NULL, &reindeer_loop, id_pointer);
    }

    pthread_t elves_t[ELVES_AMOUNT];
    for (int i = 0; i < ELVES_AMOUNT; i++){
        int* id_pointer = malloc(sizeof (int));
        *id_pointer = i;
        pthread_create(&elves_t[i], NULL, &elf_loop, id_pointer);

    }

    //santa loop
    while (true) {

        pthread_mutex_lock(&santa_mutex);

        while (elves_in_queue < ELVES_QUEUE_LENGTH && reindeer_comeback < REINDEER_AMOUNT) {
            printf("[%s-Mikolaj] Spie\n",getTime());
            pthread_cond_wait(&santa_cond, &santa_mutex);
        }

        pthread_mutex_unlock(&santa_mutex);
        printf("[%s-Mikolaj] Budze sie!\n",getTime());
        pthread_mutex_lock(&elf_mutex);
        if (elves_in_queue == ELVES_QUEUE_LENGTH) {
            printf("[%s-Mikolaj] Rozwiazuje problemy elfow %d, %d, %d\n", getTime(),elves_queue[0], elves_queue[1], elves_queue[2]);
            int sleep_time = rand() % 2 + 1;
            sleep(sleep_time);

            pthread_mutex_lock(&elf_mutex_wait);
            clear_queue(elves_queue);
            elves_in_queue = 0;
            pthread_cond_broadcast(&elf_cond_wait);
            pthread_mutex_unlock(&elf_mutex_wait);
        }
        pthread_mutex_unlock(&elf_mutex);

        pthread_mutex_lock(&reindeer_mutex);

        if (reindeer_comeback == REINDEER_AMOUNT){
            end_christmas_value+=1;
            printf("[%s-Mikolaj] Dostarczam zabawki %d\n",getTime(),end_christmas_value);
            int time_sleep = rand() % 4 +2;
            sleep(time_sleep);
            reindeer_comeback = 0;

            pthread_mutex_lock(&reindeer_mutex_wait);
            reindeer_waiting=false;
            pthread_cond_broadcast(&reindeer_cond_wait);
            pthread_mutex_unlock(&reindeer_mutex_wait);
        }
        if (end_christmas_value == 3){
            break;
        }
        printf("[%s-Mikolaj] Zasypiam \n",getTime());
        pthread_mutex_unlock(&reindeer_mutex);

    }
    printf("[%s-Mikolaj] Koniec Swiat!\n",getTime());

    printf("Simulation end: %s\n",getTime());
    return 0;
}