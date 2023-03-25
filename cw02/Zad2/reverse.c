//
// Created by Maks on 15.03.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//time
struct timespec start_realtime;
struct timespec start_usertime;
struct timespec start_systime;
struct timespec end_realtime;
struct timespec end_usertime;
struct timespec end_systime;

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


FILE *openFile(char *filepath, char *mode) {
    FILE *file = fopen(filepath, mode);
    if (file == NULL) {
        printf("%s\n", "Błąd przy otwieraniu pliku");
        exit(1);
    }
    return file;
}

void closeFile(FILE *file) {
    fclose(file);
}

size_t get_file_size(FILE *file) {
    int result = fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

void reverse(FILE *old_file, FILE *new_file) {
    start_clock();
    size_t size = get_file_size(old_file);
    char *buffer = calloc(size, sizeof(char));

    int read_result = fread(buffer, 1, size, old_file);
    if (read_result != size) {
        printf("Błąd fread\n");
        free(buffer);
        return;
    }

    for (int i = size - 1; i >= 0; i--) {
        if (buffer[i] == '\n' && buffer[i + 1] == '\n') {
            continue;
        }

        int write_result = fwrite(&buffer[i], 1, 1, new_file);
        if (write_result != 1) {
            printf("Błąd fwrite\n");
            free(buffer);
            return;
        }


    }
    free(buffer);
    end_clock();
}

void reverse1024(FILE *old_file, FILE *new_file) {
    const int block_size = 1024;
    char buffer[block_size];
    char buffer2[block_size];
    long total_bytes_read = 0;

    start_clock();
    while (1) {
        size_t bytes_read = fread(buffer, 1, block_size, old_file);
        if (bytes_read == 0) {
            break;
        }
        total_bytes_read += bytes_read;

        for (size_t i = 0; i < bytes_read; i++) {
            buffer2[i] = buffer[bytes_read - i - 1];
        }

        int write_result = fwrite(buffer2, 1, bytes_read, new_file);
        if (write_result != bytes_read) {
            printf("Błąd fwrite\n");
            return;
        }
    }
    end_clock();

}


int main(int argc, char *argv[]){

    char* old_file_path = argv[1];
    char* new_file_path = argv[2];

//    char old_file_path[100];
//    char new_file_path[100];
//
//    printf("%s", "Podaj ścieżkę pliku do czytania: ");
//    scanf("%99s", old_file_path);
//
//    printf("%s", "Podaj ścieżkę pliku do zapisu: ");
//    scanf("%99s", new_file_path);


    FILE *old_file = openFile(old_file_path, "r");
    FILE *new_file = openFile(new_file_path, "w+");

#ifdef REVERSE1024
    reverse1024(old_file, new_file);
#else
    reverse(old_file,new_file);
#endif




    closeFile(old_file);
    closeFile(new_file);

    return 0;
}