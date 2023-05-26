#include "common.h"




int isEmpty(SharedMemory *sharedMemory) {

    int count = 0;
    for (int i = 0; i < WAITING_ROOM; ++i) {
        if (sharedMemory->queue[i] != '\0') {
            count += 1;
        }
    }
    if (count != 0) {
        return count + 1;
    }
    return count;
}

char queue_pop(SharedMemory *sharedMemory) {
    if (isEmpty(sharedMemory) == 0) {
        return '\0';
    }
    char byte = sharedMemory->queue[sharedMemory->toTake % WAITING_ROOM];
    sharedMemory->queue[sharedMemory->toTake % WAITING_ROOM] = '\0';
    sharedMemory->toTake += 1;
    return byte;
}


void queue_push(SharedMemory *sharedMemory, char byte) {
    if (isEmpty(sharedMemory) >= WAITING_ROOM) {
        return;
    }
    sharedMemory->queue[sharedMemory->toPut % WAITING_ROOM] = byte;
    sharedMemory->toPut += 1;
}


static int setSharedMemory(char *filename) {
    int key = shm_open(filename, O_CREAT | O_RDWR, 0644);
    if (key == -1) {
        return -1;
    }
    if (ftruncate(key, WAITING_ROOM) == -1) {
        perror("ftruncate() call");
        return -1;
    }
    return key;
}

SharedMemory *addSharedMemory(char *filename) {
    int shared_memory_id = setSharedMemory(filename);
    if (shared_memory_id == -1) {
        fprintf(stderr, "Can't get file descriptor for: %s\n", filename);
        return NULL;
    }
    SharedMemory *shared_memory;
    shared_memory = (SharedMemory *) mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id,
                                          0);

    return shared_memory;
}

int freeSharedMemory(SharedMemory *shared_memory) {
    int result = shmdt(shared_memory);
    if (result != -1) {
        return result;
    }
    return -1;
}

typedef sem_t *Semaphore;


Semaphore createSem(char *filename, int initial) {
    Semaphore sem = sem_open(filename, O_CREAT | O_EXCL, 0644, initial);
    if (sem == SEM_FAILED) {
        printf("Error during creating semaphore\n");
        return NULL;
    }
    return sem;
}

Semaphore openSem(char *filename) {
    Semaphore sem = sem_open(filename, 0);
    if (sem == SEM_FAILED) {
        printf("Error during open semaphore\n");
        return NULL;
    }
    return sem;
}

void unlink_sem(char *filename) {
    sem_unlink(filename);
}

int take_sem(Semaphore sem) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 8;
    int res = sem_timedwait(sem, &ts);
    return res;

}

void free_sem(Semaphore sem) {
    sem_post(sem);
}


void barber() {
    SharedMemory *queue = addSharedMemory(ID_PROJECT);
    int control = 0;
    if (queue == NULL) {
        printf("%s Barber -> Cant open queue\n", getTime());
        exit(-1);
    }

    Semaphore sem_queueB = openSem(SEM_Q);
    Semaphore sem_chairsB = openSem(SEM_C);
    Semaphore sem_barbersB = openSem(SEM_B);
    Semaphore buffer_mutexB = openSem(SEM_BM);

    printf("%s Barber -> Spawn\n", getTime());


    while (true) {
        int result = take_sem(sem_barbersB);
        if (result == -1) {
            break;
        }

        free_sem(buffer_mutexB);
        char haircut = queue_pop(queue);
        free_sem(buffer_mutexB);
        if (haircut != '\0') {
            printf("%s Barber -> Working on haircut number, %d\n", getTime(),  haircut);
            sleep(haircut / 2);
            printf("%s Barber -> Done working on haircut number, %d\n", getTime(),  haircut);
        }

        free_sem(sem_chairsB);
        free_sem(sem_queueB);
        if (isEmpty(queue) == 0) {
            sleep(3);

            if (isEmpty(queue) == 0) {
                break;
            }
        }
        control += 1;
    }
    printf("%s Barber -> End of shift\n", getTime());
    freeSharedMemory(queue);
    exit(0);
}


void client() {

    srand(getpid());
    SharedMemory *queue = addSharedMemory(ID_PROJECT);

    if (queue == NULL) {
        printf("%s Client -> Cant open queue!\n", getTime());
        exit(-1);
    }


    Semaphore sem_queueC = openSem(SEM_Q);
    Semaphore sem_chairsC = openSem(SEM_C);
    Semaphore sem_barbersC = openSem(SEM_B);
    Semaphore buffer_mutexC = openSem(SEM_BM);

    int result = take_sem(sem_queueC);
    int result1 = take_sem(buffer_mutexC);

    if ((isEmpty(queue) >= WAITING_ROOM) || result == -1 || result1 == -1) {
        printf("%s Client -> NO PLACE IN QUEUE FOR CLIENT %d!!\n", getTime(), getpid());
        free_sem(buffer_mutexC);
        free_sem(sem_barbersC);
        exit(0);
    }


    char haircut = (char) (rand() % HAIRCUTS + 1);

    printf("%s Client -> Haircut number %d\n", getTime(),  haircut);

    queue_push(queue, haircut);
    free_sem(buffer_mutexC);
    free_sem(sem_barbersC);

    take_sem(sem_chairsC);

    printf("%s Client -> Client done!\n", getTime());

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

    SharedMemory *shared = addSharedMemory(ID_PROJECT);
    for (int i = 0; i < WAITING_ROOM; i++) {
        shared->queue[i] = '\0';
    }
    shared->toTake = 0;
    shared->toPut = 0;


    unlink_sem(SEM_Q);
    unlink_sem(SEM_C);
    unlink_sem(SEM_B);
    unlink_sem(SEM_BM);

    Semaphore sem_queue = createSem(SEM_Q, CHAIRS);
    Semaphore sem_chairs = createSem(SEM_C, 0);
    Semaphore sem_barbers = createSem(SEM_B, 0);
    Semaphore buffer_mutex = createSem(SEM_BM, 1);

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
    for (int i = 0; i < CLIENTS; i++) {
        if (fork() == 0) {
            client();
        }
        sleep(2);
    }
    printf("%s Main -> Clients spawned\n", getTime());


    while (wait(NULL) > 0);


    printf("%s Main -> End simulation\n", getTime());

    return 0;
}