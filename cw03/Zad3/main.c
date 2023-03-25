#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>


#define MAX_PATTERN_SIZE 255

int main(int argc, char *argv[]) {
    char *path = argv[1];
    int len = strlen(argv[2]);

    struct stat file_info;
    struct dirent *file;
    char buffer[len + 1];

    DIR *dir = opendir(path);
    char program_path[PATH_MAX];

    sprintf(program_path, "%s%s", path, argv[0]);
    realpath(argv[0], program_path);

    while ((file = readdir(dir)) != NULL) {
        if (strcmp(file->d_name, ".") == 0) {
            continue;
        }
        if (strcmp(file->d_name, "..") == 0) {
            continue;
        }

        char full_path[PATH_MAX];
        sprintf(full_path, "%s/%s", path, file->d_name);

        if (stat(full_path, &file_info) < 0) {
            perror("stat");
            exit(EXIT_FAILURE);
        }

        if (S_ISDIR(file_info.st_mode)) {
            //jest katalogiem

            if (fork() == 0) {
                if (execl(program_path, program_path, full_path, argv[2], NULL) < 0) {
                    perror("execl");
                    exit(EXIT_FAILURE);
                }
            }
            wait(NULL);

        } else {
            FILE *fp = fopen(full_path, "r");
            if (fp == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            if (fgets(buffer, len + 1, fp) == NULL) {
                if (ferror(fp)) {
                    perror("fgets");
                    exit(EXIT_FAILURE);
                }
            }

            if (strcmp(buffer, argv[2]) == 0) {
                printf("%s %d\n", full_path, getpid());
            }

            if (fclose(fp) < 0) {
                perror("fclose");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (closedir(dir) < 0) {
        perror("closedir");
        exit(EXIT_FAILURE);
    }

    return 0;
}
