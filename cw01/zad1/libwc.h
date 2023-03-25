#ifndef LIBWC_H
#define LIBWC_H

#include <stdlib.h>



// Struktura przechowująca linie tekstu
typedef struct TextLines {
    char** lines;     // Tablica wskaźników na linie tekstu
    size_t num_lines; // Ilość zapisanych linii
} TextLines;

// Struktura przechowująca bloki pamięci
typedef struct MemoryBlocks {
    TextLines* blocks;    // Tablica  bloki pamięci
    size_t max_size;  // Maksymalna ilość bloków
    size_t curr_size; // Aktualna ilość zapisanych bloków
} MemoryBlocks;



MemoryBlocks *initMemoryBlocks(size_t max_size);
void count(MemoryBlocks *mem_blocks, char *open_filename);
void show(MemoryBlocks *mem_blocks, size_t index);
void delete(MemoryBlocks *mem_blocks, size_t index);
void destroy(MemoryBlocks *memoryBlocks);





#endif /* LIBWC_H */
