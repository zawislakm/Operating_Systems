#include "common.h"



int isEmpty(SharedMemory *sharedMemory){
    int count =0;
    for(int i = 0; i < WAITING_ROOM;++i){
        if (sharedMemory->queue[i] != '\0'){
            count+=1;
        }
    }
    return count;
}

char queue_pop(SharedMemory *sharedMemory){
    if (isEmpty(sharedMemory)==0){
        return '\0';
    }
    char byte = sharedMemory->queue[sharedMemory->toTake%WAITING_ROOM];
    sharedMemory->queue[sharedMemory->toTake%WAITING_ROOM] = '\0';
    sharedMemory->toTake+=1;
    return byte;
}

void queue_push(SharedMemory *sharedMemory,char byte){
    if (isEmpty(sharedMemory) >= WAITING_ROOM){
        return;
    }
    sharedMemory->queue[sharedMemory->toPut%WAITING_ROOM] = byte;
    sharedMemory->toPut+=1;
}


static int setSharedMemory(char *filename) {
    key_t key = ftok(filename, 0);
    if (key == -1) {
        printf("Ftok error in get shared memory\n");
        return -1;
    }
    return shmget(key, WAITING_ROOM, 0644 | IPC_CREAT);
}

SharedMemory *addSharedMemory(char *filename) {
    int shared_memory_id = setSharedMemory(filename);
    if (shared_memory_id == -1){
        printf("Attaching shared memory error1!\n");
        return NULL;
    }
    SharedMemory *shared_memory;
    shared_memory = shmat(shared_memory_id, NULL, 0);
    if (shared_memory == (SharedMemory *)(-1)){
        printf("Attaching shared memory error2!\n");
        return NULL;
    }

    return shared_memory;
}

int freeSharedMemory(SharedMemory *shared_memory){
    int result = shmdt(shared_memory);
    if (result != -1){
        return result;
    }
    return -1;
}

typedef int Semaphore;


Semaphore createSem(char *filename, int initial) {
    key_t key = ftok(getenv("HOME"), filename[0]); //-1

    if (key == -1){
        printf("Error ftok during creating semaphore\n");
        return -1;
    }

    Semaphore semid = semget(key, 1, 0664 | IPC_CREAT); // -1
    if (semid == -1){
        printf("Error semget durgin creating semaphore\n");
        return -1;
    }

    if (semctl(semid, 0,SETVAL, initial) == -1){
        printf("Error semctl durgin creating semaphore\n");
        return -1;
    }
    return semid;
}

Semaphore openSem(char *filename) {
    key_t key = ftok(getenv("HOME"), filename[0]); //-1
    if (key == -1) {
        printf("Error ftok drugin opening semaphore\n");
        return -1;
    }
    return semget(key, 1, 0);
}

void take_sem(Semaphore sem){
    struct sembuf operation = {0,-1,0};
    if (semop(sem,&operation,1) == -11){
        printf("Take sem error\n");
    }
}

void free_sem(Semaphore sem){
    struct sembuf operation = {0,1,0};
    if (semop(sem,&operation,1) == 1){
        printf("Free sem error\n");
    }
}



void barber(){
    SharedMemory * queue = addSharedMemory(ID_PROJECT);

    if (queue ==NULL){
        printf("%s Barber -> Cant open queue\n",getTime());
        exit(-1);
    }
    Semaphore sem_queueB = openSem(SEM_Q);
    Semaphore sem_chairsB = openSem(SEM_C);
    Semaphore sem_barbersB = openSem(SEM_B);
    Semaphore buffer_mutexB = openSem(SEM_BM);

    printf("%s Barber -> Spawn\n",getTime());


    while (true){

        take_sem(sem_barbersB);

        free_sem(buffer_mutexB);
        char haircut = queue_pop(queue);
        free_sem(buffer_mutexB);

        if (haircut != '\0'){
            printf("%s Barber -> Working on haircut number, %d\n",getTime(),haircut);
            usleep(haircut*1000);
            printf("%s Barber -> Done working on haircut number, %d\n",getTime(),haircut);
        }

        free_sem(sem_chairsB);
        free_sem(sem_queueB);

        if (isEmpty(queue)==0){
            usleep(10000000);
            if (isEmpty(queue)==0){
                break;
            }
        }
    }
    printf("%s Barber -> End of shift\n",getTime());
    freeSharedMemory(queue);
    exit(0);
}



void client(){

    srand(getpid());
    SharedMemory *queue = addSharedMemory(ID_PROJECT);

    if (queue == NULL){
        printf("\t[%s Client -> Cant open queue!\n",getTime());
        exit(-1);
    }


    Semaphore sem_queueC = openSem(SEM_Q);
    Semaphore sem_chairsC = openSem(SEM_C);
    Semaphore sem_barbersC = openSem(SEM_B);
    Semaphore buffer_mutexC = openSem(SEM_BM);


    if ((isEmpty(queue) >= WAITING_ROOM-1)){
        printf("%s  NO PLACE IN QUEUE FOR CLIENT %d!!\n",getTime(),getpid());
        exit(0);
    }
    take_sem(sem_queueC);
    take_sem(buffer_mutexC);

    char haircut = (char) (rand()%HAIRCUTS+1);

    printf("%s Client -> Haircut number %d\n",getTime(),haircut);

    queue_push(queue,haircut);
    free_sem(buffer_mutexC);
    free_sem(sem_barbersC);

    take_sem(sem_chairsC);

    printf("%s Client -> Client done!\n",getTime());

    freeSharedMemory(queue);
    exit(0);
}



int main() {

    localtime(&timestamp);
    printf("%s Main\n", getTime());
    printf("Barbers: %d\n", BARBERS);
    printf("Chairs: %d\n", CHAIRS);
    printf("Clients: %d\n", CLIENTS);
    printf("\n");
    sleep(1);
    printf("Start \n");

    SharedMemory *shared = addSharedMemory(ID_PROJECT); //NULL
    for(int i =0;i<WAITING_ROOM;i++){
        shared->queue[i] = '\0';
    }
    shared->toTake=0;
    shared->toPut=0;

    Semaphore sem_queue = createSem(SEM_Q,CHAIRS);
    Semaphore sem_chairs = createSem(SEM_C,0);
    Semaphore sem_barbers = createSem(SEM_B,0);
    Semaphore buffer_mutex = createSem(SEM_BM,1);

    if (fork() == 0){
        for (int i = 0; i < BARBERS; i++) {
            if (fork() == 0) {
                barber();
            }
        }
        sleep(1);
        printf("%s Main-Barber Fork -> Barbers spawned\n", getTime());
        while (wait(NULL) > 0);
        return 0;
    }
    sleep(2);
    for(int i =0;i<CLIENTS;i++){
        if (fork()==0){
            client();
        }
        sleep(2);
    }
    printf("%s Main -> Clients spawned\n", getTime());


    while (wait(NULL)>0);

    printf("%s Main -> End simulation\n",getTime());

    return 0;
}