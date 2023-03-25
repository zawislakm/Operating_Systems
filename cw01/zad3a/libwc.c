#include "libwc.h"
#include <stdio.h>
#include <string.h>


MemoryBlocks *initMemoryBlocks(size_t max_size) {

    MemoryBlocks *mem_blocks = calloc(2000, sizeof(MemoryBlocks));
    mem_blocks->blocks = (TextLines *) calloc(max_size, sizeof(TextLines));

    if (mem_blocks->blocks == NULL) {
        printf("Out of memory, during creating memory blocks");
        exit(1);
    }
    mem_blocks->max_size = max_size;
    mem_blocks->curr_size = 0;
    return mem_blocks;
}

void show(MemoryBlocks *mem_blocks, size_t index) {
    if (index >= mem_blocks->curr_size) {
        printf("Błąd: indeks poza zakresem\n");
        return;
    }

    TextLines textLines = (mem_blocks->blocks[index]);
    printf("\n");
    for (int t = 0; t < textLines.num_lines; t++) {
        printf("%s", textLines.lines[t]);
    }
}


void delete(MemoryBlocks *mem_blocks, size_t index) {
    if (index >= mem_blocks->curr_size) {
        printf("Błąd: indeks bloku poza zakresem.\n");
        return;
    }

    TextLines *block_to_remove = &mem_blocks->blocks[index];
    if (block_to_remove == NULL) {
        printf("Błąd: nie znaleziono bloku o podanym indeksie.\n");
        return;
    }

    if (block_to_remove->lines == 0) {
        return;
    }

    for (size_t i = 0; i < block_to_remove->num_lines; i++) {//usuwa linia po lini
        free(block_to_remove->lines[i]);
    }
    free(block_to_remove->lines); //usuwa wskazniki
    block_to_remove->lines = NULL;
    block_to_remove->num_lines = 0;
}

void destroy(MemoryBlocks *memoryBlocks) {
    for (size_t i = 0; i < memoryBlocks->curr_size; i++) {
        delete(memoryBlocks, i);
    }

    free(memoryBlocks->blocks);
    free(memoryBlocks);

}

void add_memory_block(MemoryBlocks *mem_blocks, char **lines, size_t num_lines) {
    if (mem_blocks->curr_size == mem_blocks->max_size) {
        printf("Błąd: osiągnieto maksymalna ilość miejsca\n");
        return;
    }
    TextLines *new_block = mem_blocks->blocks + mem_blocks->curr_size;
    mem_blocks->curr_size++;
    new_block->lines = lines;
    new_block->num_lines = num_lines;
}

int count_lines_in_wc_output_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Nie można otworzyć pliku: %s\n", filename);
        return 0;
    }
    int lines = 0;
    if (fscanf(file, "%d", &lines) != 1) { // Sprawdź, czy fscanf zwrócił 1 (poprawnie odczytana wartość)
        printf("Błąd odczytu liczby linii z pliku: %s\n", filename);
        fclose(file);
        return 0;
    }
    fclose(file);
    return lines;
}

void count(MemoryBlocks *mem_blocks, char *open_filename) {
    int result = system("mkdir -p tmp ");

    char filename[] = "tmp/tmp.txt";
    FILE *fp = fopen("tmp/tmp.txt", "w+");


    fclose(fp);
    char *command = calloc(10000, sizeof(char));
    sprintf(command, "wc %s > %s", open_filename, filename);
    system(command);
    int lines_count = count_lines_in_wc_output_file(filename);
    sprintf(command, "rm -f %s", filename);
    system(command);

    fp = fopen(open_filename, "r");
    if (fp == NULL) {
        printf("Something went wrong during opening the open_filename");
        return;
    }

    char **lines = (char **) calloc(lines_count, sizeof(char *));
    int i = 0;
    while (1) {

        char *temp_line = calloc(10000, sizeof(char));
        size_t len = 0;
        size_t characters = getline(&temp_line, &len, fp);
        if (characters == EOF) break;
        *(lines + i) = realloc(temp_line, len * sizeof(char));
        i++;
    }


    fclose(fp);

    add_memory_block(mem_blocks, lines, lines_count);


}


