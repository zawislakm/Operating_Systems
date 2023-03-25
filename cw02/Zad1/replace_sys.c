//
// Created by Maks on 13.03.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define LINE_LENGTH 1024

int openFile(char* filepath, int flags) {
    int file = open(filepath, flags);
    if (file == -1){
        printf("%s\n","Błąd przy otwieraniu pliku");
        exit(1);
    }
    return file;
}

void closeFile(int file){
    close(file);
}

void replace_lines(int old_file, int new_file, char old_char, char new_char){
    char c;
    int write_check;

    while (read(old_file,&c,1)==1){
        if (c == old_char){
            c = new_char;
        }
        write_check = write(new_file,&c,1);
        if (write_check == -1){
            printf("%s","Błąd przy zapisie lini");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]){



    char old_char = argv[1][0];
    char new_char = argv[2][0];
    char *old_file_path = argv[3];
    char *new_file_path = argv[4];

    int old_file = openFile(old_file_path, O_RDONLY);
    int new_file = openFile(new_file_path, O_WRONLY | O_CREAT | O_TRUNC);


    replace_lines(old_file,new_file,old_char,new_char);
    closeFile(old_file);
    closeFile(new_file);

    return 0;
}

