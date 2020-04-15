#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("wrong syntax");
        exit(1);
    }

    FILE *pipe = fopen(argv[1], "r");
    FILE *file = fopen(argv[2], "w+");
    int N = atoi(argv[3]);

    char *buff = calloc(N + 1, sizeof(char));
    while (fread(buff, 1, N, pipe) > 0) {
        fwrite(buff, 1, N, file);
    }

    free(buff);
    fclose(file);
    fclose(pipe);
}