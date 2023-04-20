#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/*
 * Funkcja 'read_end' powinna:
 *  - otworzyc plik o nazwie przekazanej w argumencie
 *    'file_name' w trybie tylko do odczytu,
 *  - przeczytac ostatnie 8 bajtow tego pliku i zapisac
 *    wynik w argumencie 'result'.
 */
void read_end(char *file_name, char *result){
    // Uzupelnij cialo funkcji read_end zgodnie z
    // komentarzem powyzej

    int file = open(file_name, O_RDONLY);
    if (file == -1) {
        printf("Open error\n");
        return;
    }

    struct stat st;
    if (fstat(file, &st) == -1) {
        printf("Fstat error\n");
        close(file);
        return;
    }
    off_t size = st.st_size;

    if (lseek(file, size - 8, SEEK_SET) == -1) {
        printf("Lseek error\n");
        close(file);
        return;
    }

    ssize_t n = read(file, result, 8);
    if (n == -1) {
        printf("Read error \n");
        close(file);
        return;
    }


    if (close(file) == -1) {
        printf("Close error\n");
        return;
    }
}


int main(int argc, char *argv[]) {
    int result[2];

    if (argc < 2) return -1;
    read_end(argv[1], (char *) result);
    printf("magic number: %d\n", (result[0] ^ result[1]) % 1000);
    return 0;
}



