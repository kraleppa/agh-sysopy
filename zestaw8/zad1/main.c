#define _POSIX_C_SOURCE 199309L
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_LINE_LENGTH 70
#define EMPTYSPACE " \t\r\n"
#define MAX_COLOR 256



unsigned char **img;
int height;
int width;

int **hist_pieces;

int threads_number;

int time_measure(struct timespec *start){
    struct timespec stop;
    clock_gettime(CLOCK_MONOTONIC, &stop);
    int res = (stop.tv_sec - start->tv_sec) * 1000000;
    res += (stop.tv_nsec - start->tv_nsec) / 1000.0;
    return res;
}

void read_line(char *buff, FILE *file){
    do {
        fgets(buff, MAX_LINE_LENGTH, file);
    } while (buff[0] == '#' || buff[0] == '\n');
}

void read_img(char *file_name){
    FILE *file = fopen(file_name, "r");
    char buff[MAX_LINE_LENGTH + 1] = {0};
    read_line(buff, file);
    read_line(buff, file);
    width = atoi(strtok(buff, EMPTYSPACE));
    height = atoi(strtok(NULL, EMPTYSPACE));
    img = calloc(height, sizeof(char *));

    for (int i = 0; i < height; i++) {
        img[i] = calloc(width, sizeof(char));
    }
    read_line(buff, file);
    read_line(buff, file);

    char *value_encoded = strtok(buff, EMPTYSPACE);
    for (int i = 0; i < width * height; i++){
        if (value_encoded == NULL){
            read_line(buff, file);
            value_encoded = strtok(buff, EMPTYSPACE);
        }
        img[i / width][i % width] = atoi(value_encoded);
        value_encoded = strtok(NULL, EMPTYSPACE);
    }
    fclose(file);
}

void save_hist(char *file_name){
    FILE *file = fopen(file_name, "w+");
    int hist[MAX_COLOR];
    for (int i = 0; i < MAX_COLOR; i++){
        hist[i] = 0;
    }
    for (int i = 0; i < threads_number; i++){
        for (int j = 0; j < MAX_COLOR; j++){
            hist[j] += hist_pieces[i][j];
        }
    }
    float max_occ = hist[0];
    for (int i = 1; i < MAX_COLOR; i++) {
        if (max_occ < hist[i]) {
            max_occ = hist[i];
        }
    }

    fputs("P2\n", file);
    fputs("256 50\n", file);
    fputs("255\n", file);
    for (int w = 0; w < 50; w++) {
        for (int k = 0; k < MAX_COLOR; k++) {
            if (50 - w > 50 * (hist[k] / max_occ)) {
                fputs("0\n", file);
            } else {
                fprintf(file, "255\n");
            }
        }
    }

    fclose(file);
}

int sign_thread(int *thread){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int chunk_size = MAX_COLOR / threads_number;
    for (int w = 0; w < height; w++) {
        for (int k = 0; k < width; k++) {
            if (img[w][k] / chunk_size == *thread) {
                hist_pieces[0][img[w][k]]++;
            }
        }
    }
    return time_measure(&start);
}

int block_thread(int *thread){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int chunk_size = width / threads_number;
    int th = *thread;

    for (int w = th * chunk_size; w < (th + 1) * chunk_size; w++) {
        for (int k = 0; k < height; k++) {
            hist_pieces[th][img[k][w]]++;
        }
    }
    return time_measure(&start);
}

int interleaved_thread(int *thread) {
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int th = *thread;
    for (int w = th; w < width; w += threads_number) {
        for (int k = 0; k < height; k++) {
            hist_pieces[th][img[k][w]]++;
        }
    }
    return time_measure(&start);
}

int main(int argc, char *argv[]){
    printf("EEEE");

    if (argc != 5){
        perror("Syntax error :<\n");
        exit(1);
    }

    threads_number = atoi(argv[1]);
    char *mode = argv[2];
    char *intput_file = argv[3];
    char *output_file = argv[4];
    
    read_img(intput_file);
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    hist_pieces = calloc(threads_number, sizeof(int *));
    for (int i = 0; i < threads_number; i++) {
        hist_pieces[i] = calloc(MAX_COLOR, sizeof(int));
    }

    

    pthread_t *threads = calloc(threads_number, sizeof(pthread_t));
    int *args = calloc(threads_number, sizeof(int));

    for (int i = 0; i < threads_number; i++) {
        
        int (*start)(int *);
        if (strcmp(mode, "sign") == 0) {
            start = sign_thread;
        } 

        if (strcmp(mode, "block") == 0) {
            start = block_thread;
        }

        if (strcmp(mode, "interleaved") == 0) {
            start = interleaved_thread;
        }

        args[i] = i;
        pthread_create(&threads[i], NULL, (void *(*)(void *))start, args + i);
    }

    for (int i = 0; i < threads_number; i++) {
        int time;
        pthread_join(threads[i], (void *)&time);
        printf("thread: %d time: %d ms\n", i, time);
    }

    printf("total time: %d ms\n", time_measure(&start));
    save_hist(output_file);
    free(threads);
    free(args);
    for (int i = 0; i < threads_number; i++) {
        free(hist_pieces[i]);
    }
    free(hist_pieces);
    for (int i = 0; i < height; i++) {
        free(img[i]);
    }
    free(img);
}
