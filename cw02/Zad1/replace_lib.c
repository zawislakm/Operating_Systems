#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LENGTH 1024

FILE* openFile(char* filepath, char* mode){
    FILE* file  = fopen(filepath,mode);
    if (file == NULL){
        printf("%s\n","Błąd przy otwieraniu pliku");
        exit(1);
    }
    return file;
}

void closeFile(FILE* file){
    fclose(file);
}

void replace_lines(FILE* old_file, FILE* new_file, char old_char, char new_char) {
    char c;
    size_t read_size;

    while ((read_size = fread(&c, 1, 1, old_file)) == 1) {
        if (c == old_char) {
            c = new_char;
        }

        if (fwrite(&c, 1, 1, new_file) != 1) {
            printf("Błąd przy zapisie linii\n");
            exit(1);
        }
    }

}


int main(int argc, char *argv[]){



    char old_char = argv[1][0];
    char new_char = argv[2][0];
    char *old_file_path = argv[3];
    char *new_file_path = argv[4];


    FILE* old_file = openFile(old_file_path,"r");
    FILE* new_file = openFile(new_file_path,"w+");

    replace_lines(old_file,new_file,old_char,new_char);
    closeFile(old_file);
    closeFile(new_file);

    return 0;
}
