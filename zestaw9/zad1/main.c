#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define true 1
#define false 0
pthread_mutex_t seats_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barber_ready = PTHREAD_COND_INITIALIZER;

int is_barber_sleeping;
int *seats;

int K, N;
int clients_waiting;

int get_client_index(){
    for (int i = 0; i < K; i++){
        if (seats[i] != -1){
            clients_waiting--;
            return i;
        }
    }
    return -1;
}

void client_sit(int *id){
    for (int i = 0; i < K; i++){
        if (seats[i] == -1) {
            seats[i] = *id;
            clients_waiting++;
            return;
        }
    }
}

void barber(){
    int served_customers = 0;

    while (served_customers < N){
        pthread_mutex_lock(&seats_mutex);
        
        while (clients_waiting == 0){
            printf("Golibroda: ide spac 😴\n");
            is_barber_sleeping = true;
            pthread_cond_wait(&barber_ready, &seats_mutex);
        }

        is_barber_sleeping = false;
        
        int client_index = get_client_index();

        printf("Golibroda: czeka %d klientow, gole klienta %d\n", clients_waiting, seats[client_index]);
        seats[client_index] = -1;
        served_customers++;
        pthread_mutex_unlock(&seats_mutex);
        sleep(rand() % 3 + 1);
    }
    printf("Golibroda: koniec pracy 😀\n");
}

void client(int *id){
    pthread_mutex_lock(&seats_mutex);
    if (clients_waiting == K){
        printf("Zajete; %d\n", *id);
        pthread_mutex_unlock(&seats_mutex);
        sleep(rand() % 3 + 1);
        client(id);
        return;
    }
    printf("Poczekalnia, wolne miejsca: %d; %d\n", K - clients_waiting, *id);
    client_sit(id);
    
    if (is_barber_sleeping){
        printf("Budze golibrode; %d 😮\n", *id);
        pthread_cond_broadcast(&barber_ready);
    }
    pthread_mutex_unlock(&seats_mutex);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        perror("Wrong syntax\n");
        exit(1);
    }
    K = atoi(argv[1]);
    N = atoi(argv[2]);

    seats = calloc(K, sizeof(int));
    for (int i = 0; i < K; i++){
        seats[i] = -1;
    }

    clients_waiting = 0;


    pthread_t barber_thread;
    pthread_create(&barber_thread, NULL, (void* (*)(void*))barber, NULL);
    pthread_t* customer_threads = calloc(N, sizeof(pthread_t));


    srand(time(NULL));
    int* ids = calloc(N, sizeof(int));
    for (int i = 0; i < N; i++) {
        sleep(rand() % 1 + 1);
        ids[i] = i;
        pthread_create(&customer_threads[i], NULL, (void* (*)(void*))client, ids + i);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(customer_threads[i], NULL);
    }
    pthread_join(barber_thread, NULL);

    free(ids);
    free(customer_threads);
    free(seats);
}

    
