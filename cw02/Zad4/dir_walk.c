//
// Created by Maks on 13.03.2023.
//

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <ftw.h>
#include <sys/stat.h>

static long long total_dirs_size = 0;

int reaction(const char* file,const struct stat *file_info,int flag){
    if (flag != FTW_D){

        printf("%s %ld\n",file,file_info->st_size);
        total_dirs_size+= file_info->st_size;
    }

    return 0;
}

int main(int argc, char *argv[]){

    char* dir_path = argv[1];

    int result = ftw(dir_path, reaction,1);

    if (result == -1){
        printf("%s\n","FTW nie zadziałało poprawnie");
        exit(1);
    }

    printf("%s %lld\n", "Rozmiar plików: ",total_dirs_size);
    return 0;
}