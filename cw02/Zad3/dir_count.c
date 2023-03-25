//
// Created by Maks on 13.03.2023.
//

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>



int main(int argc, char *argv[]){

    char* dir_path = argv[1];

    DIR* dir = opendir(dir_path);

    if (dir == NULL){
        printf("%s\n","Nie można otworzyć katalogu");
        exit(-1);
    }

    struct dirent* file;
    long long total_dir_size = 0;
    struct stat file_info;

    while ((file = readdir(dir)) != NULL){

        stat(file->d_name,&file_info);

        if(!(S_ISDIR(file_info.st_mode))){

            printf("%s %ld\n",file->d_name,file_info.st_size);
            total_dir_size += file_info.st_size;
        }
    }

    printf("%s %lld\n", "Rozmiar plików: ",total_dir_size);
    closedir(dir);
    return 0;
}