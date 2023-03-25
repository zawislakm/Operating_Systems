

#include <stdlib.h>


// Struktura przechowująca linie tekstu
typedef struct TextLines {
    char** lines;
    size_t num_lines;
} TextLines;

// Struktura przechowująca bloki pamięci
typedef struct MemoryBlocks {
    TextLines* blocks;
    size_t max_size;
    size_t curr_size;
} MemoryBlocks;



MemoryBlocks *initMemoryBlocks(size_t max_size);
void count(MemoryBlocks *mem_blocks, char *open_filename);
void show(MemoryBlocks *mem_blocks, size_t index);
void delete(MemoryBlocks *mem_blocks, size_t index);
void destroy(MemoryBlocks *memoryBlocks);
void load_dll();



