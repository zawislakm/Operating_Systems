


#include "libwc.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>




#define BUFFERSIZE 10000


MemoryBlocks* mem_block = NULL;
bool is_initialised = false;

// time
struct timespec start_realtime;
struct timespec start_usertime;
struct timespec start_systime;
struct timespec end_realtime;
struct timespec end_usertime;
struct timespec end_systime;



void start_clock(){
    clock_gettime(CLOCK_REALTIME, &start_realtime);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_usertime);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start_systime);
}

void end_clock(){
    clock_gettime(CLOCK_REALTIME, &end_realtime);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_usertime);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end_systime);

    long elapsed_realtime_ns = (end_realtime.tv_sec - start_realtime.tv_sec) * 1000000000 + (end_realtime.tv_nsec - start_realtime.tv_nsec);
    long elapsed_usertime_ns = (end_usertime.tv_sec - start_usertime.tv_sec) * 1000000000 + (end_usertime.tv_nsec - start_usertime.tv_nsec);
    long elapsed_systime_ns = (end_systime.tv_sec - start_systime.tv_sec) * 1000000000 + (end_systime.tv_nsec - start_systime.tv_nsec);

    printf("Czas rzeczywisty: %ld ns\n", elapsed_realtime_ns);
    printf("Czas użytkownika: %ld ns\n", elapsed_usertime_ns);
    printf("Czas systemowy: %ld ns\n", elapsed_systime_ns);
}

void init_command(size_t size){
    if( is_initialised){
        printf("%s\n","ITS ALREADY INITIALIZED, RUN destroy BEFORE NEXT init");
        return;
    }else{
        start_clock();
        mem_block = initMemoryBlocks(size);
        end_clock();
        is_initialised = true;
        return;
    }
}

void count_command(char* filename){
    if (!(is_initialised)){
        printf("%s\n","RUN init BEFORE count");
        return;
    }
    start_clock();
    count(mem_block,filename);
    end_clock();
    return;
}

void show_command(int index){
    if (!(is_initialised)){
        printf("%s\n","RUN init && count BEFORE show");
        return;
    }
    start_clock();
    show(mem_block,index);
    end_clock();
}

void delete_index_command(int index_del){
    if (!(is_initialised)){
        printf("%s\n","RUN init && count BEFORE delete index");
        return;
    }
    start_clock();
    delete(mem_block,index_del);
    end_clock();
}

void destroy_command(){
    if (!(is_initialised)){
        printf("%s\n","RUN init  BEFORE destroy");
        return;
    }
    start_clock();
    destroy(mem_block);
    end_clock();
    is_initialised= false;
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

    if (strcmp(key_word, "delete") == 0 && strcmp(second_part, "index") == 0){
        int index_del;
        sscanf(command,"delete index %d",&index_del);
        delete_index_command(index_del);
        return;
    }

    if(strcmp(key_word,"destroy")==0){
        destroy_command();
        return;
    }

    printf("%s\n", "WRONG COMMAND");


}


int main() {

    char buffer[BUFFERSIZE];

    printf("Repl>");
    while (*(fgets(buffer, BUFFERSIZE, stdin)) != '\n') {
        parse_input(buffer);
        printf("Repl>");
    }

    return 0;
}
