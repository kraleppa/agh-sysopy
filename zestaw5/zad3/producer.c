#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("wrong syntax");
        exit(1);
    }

    FILE *pipe = fopen(argv[1], "w");
    FILE *file = fopen(argv[2], "r");
    int N = atoi(argv[3]);

    char *buff = calloc(N + 1, sizeof(char));
    while (fread(buff, 1, N, file) > 0) {
        sleep(rand() % 2 + 1);
        fprintf(pipe, "#%d#%s\n", getpid(), buff);
        fflush(pipe);
    }

    free(buff);
    fclose(file);
    fclose(pipe);
}