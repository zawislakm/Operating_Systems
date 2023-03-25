

#include <dlfcn.h>
#include "libwc.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

//use only for dllopen
typedef void *(*arbitrary)();


#define BUFFERSIZE 10000


MemoryBlocks *mem_block = NULL;
bool is_initialised = false;

// time
struct timespec start_realtime;
struct timespec start_usertime;
struct timespec start_systime;
struct timespec end_realtime;
struct timespec end_usertime;
struct timespec end_systime;

void *handle;

void start_clock() {
    clock_gettime(CLOCK_REALTIME, &start_realtime);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_usertime);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start_systime);
}

void end_clock() {
    clock_gettime(CLOCK_REALTIME, &end_realtime);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_usertime);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end_systime);

    long elapsed_realtime_ns = (end_realtime.tv_sec - start_realtime.tv_sec) * 1000000000 +
                               (end_realtime.tv_nsec - start_realtime.tv_nsec);
    long elapsed_usertime_ns = (end_usertime.tv_sec - start_usertime.tv_sec) * 1000000000 +
                               (end_usertime.tv_nsec - start_usertime.tv_nsec);
    long elapsed_systime_ns =
            (end_systime.tv_sec - start_systime.tv_sec) * 1000000000 + (end_systime.tv_nsec - start_systime.tv_nsec);

    printf("\n");
    printf("Czas rzeczywisty: %ld ns\n", elapsed_realtime_ns);
    printf("Czas użytkownika: %ld ns\n", elapsed_usertime_ns);
    printf("Czas systemowy: %ld ns\n", elapsed_systime_ns);
}

void init_command(size_t size) {
    if (is_initialised) {
        printf("%s\n", "ITS ALREADY INITIALIZED, RUN destroy BEFORE NEXT init");
        return;
    } else {
        start_clock();
        arbitrary finitMemoryBlocks;
        *(void **) (&finitMemoryBlocks) = dlsym(handle, "initMemoryBlocks");
        mem_block = finitMemoryBlocks(size);
        end_clock();
        is_initialised = true;
        return;
    }
}

void count_command(char *filename) {
    if (!(is_initialised)) {
        printf("%s\n", "RUN init BEFORE count");
        return;
    }
    start_clock();
    arbitrary fcount;
    *(void **) (&fcount) = dlsym(handle, "count");
    fcount(mem_block, filename);
    end_clock();
    return;
}

void show_command(int index) {
    if (!(is_initialised)) {
        printf("%s\n", "RUN init && count BEFORE show");
        return;
    }
    start_clock();
    arbitrary fshow;
    *(void **) (&fshow) = dlsym(handle, "show");
    fshow(mem_block, index);
    end_clock();
}

void delete_index_command(int index_del) {
    if (!(is_initialised)) {
        printf("%s\n", "RUN init && count BEFORE delete index");
        return;
    }
    start_clock();
    arbitrary fdelete;
    *(void **) (&fdelete) = dlsym(handle, "delete");
    fdelete(mem_block, index_del);
    end_clock();
}

void destroy_command() {
    if (!(is_initialised)) {
        printf("%s\n", "RUN init  BEFORE destroy");
        return;
    }
    start_clock();
    arbitrary fdestroy;
    *(void **) (&fdestroy) = dlsym(handle, "destroy");
    fdestroy(mem_block);
    end_clock();
    is_initialised = false;
}


void parse_input(char command[BUFFERSIZE]) {
    char key_word[BUFFERSIZE];
    char second_part[BUFFERSIZE];

    sscanf(command, "%s %s", key_word, second_part);


    if (strcmp(key_word, "init") == 0) {
        int size;
        sscanf(second_part, "%d", &size);
        init_command(size);

        return;
    }

    if (strcmp(key_word, "count") == 0) {
        char *filename = second_part;
        count_command(filename);
        return;
    }

    if (strcmp(key_word, "show") == 0) {
        int index;
        sscanf(second_part, "%d", &index);
        show_command(index);
        return;
    }

    if (strcmp(key_word, "delete") == 0 && strcmp(second_part, "index") == 0) {
        int index_del;
        sscanf(command, "delete index %d", &index_del);
        delete_index_command(index_del);
        return;
    }

    if (strcmp(key_word, "destroy") == 0) { ;
        destroy_command();
        return;
    }

    if (strcmp(key_word, "exit") == 0) {
        exit(0);
    }
    printf("%s\n", "WRONG COMMAND");
    return;


}


int main() {

    handle = dlopen("libwc.so", RTLD_LAZY);


    char buffer[BUFFERSIZE];

    printf("Repl>");
    while (*(fgets(buffer, BUFFERSIZE, stdin)) != '\n') {
        //printf("%s", buffer);
        parse_input(buffer);
        printf("Repl>");
    }

    dlclose(handle);
    return 0;
}
