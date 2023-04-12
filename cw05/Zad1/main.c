#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char *argv[]) {

    FILE *file;
    char command[100];
    if (argc == 2) {
        printf("%s\n", "SHOW");

        if (strcmp(argv[1], "date") == 0) {
            snprintf(command, sizeof(command), "echo | mail | tail +2 | head -n -1 | tac ");
        } else if (strcmp(argv[1], "sender") == 0) {
            snprintf(command, sizeof(command), "echo | mail | tail +2 | head -n -1 | sort -k 2");
        } else {
            return 0;
        }

        file = popen(command, "r");
        if (file == NULL) {
            printf("File popen error\n");
            exit(1);
        }
        printf("%s\n", "Output:");

        char line[1024];
        while (fgets(line, 1024, file) != NULL) {
            printf("%s", line);
        }

        pclose(file);
        return 0;
    }

    if (argc == 4) {
        printf("%s\n", "SEND");
        snprintf(command, sizeof(command), "echo %s | mail -s %s %s", argv[1], argv[2], argv[3]);
        file = popen(command, "r");
        if (file == NULL) {
            printf("File popen error\n");
            exit(1);
        }
        pclose(file);
        return 0;
    }

    return 0;
}
