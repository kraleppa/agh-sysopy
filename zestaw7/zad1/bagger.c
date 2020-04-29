#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#define SPACE_INDEX 0
#define CREATED_INDEX 1
#define PACKED_INDEX 2
#define CAN_MODIFY_INDEX 3
#define MAX_CREATED_COUNT 10
#define PACKAGES_COUNT 10


typedef enum { CREATED, PACKED, SENT } package_status;
typedef struct {
    package_status status;
    int value;
} package_t;


typedef struct {
    int index;
    int size;
    package_t packages[PACKAGES_COUNT];
} memory_t;

int main(){
    key_t key = ftok("main", 1);

    int semaphor_set = semget(key, 4, 0);
    int mem = shmget(key, sizeof(memory_t), 0);
    struct sembuf ops_start[2] = {{CAN_MODIFY_INDEX, -1, 0}, {CREATED_INDEX, -1, 0}};
    struct sembuf ops_end[2] = {{CAN_MODIFY_INDEX, 1, 0}, {PACKED_INDEX, 1, 0}};

    while(1){
        semop(semaphor_set, ops_start, 2);
        memory_t *m = shmat(mem, NULL, 0);

        int i = m -> index;

        while (m->packages[i].status != CREATED) {
            i = (i + 1) % PACKAGES_COUNT;
        }
        m->packages[i].status = PACKED;
        m->packages[i].value *= 2;
        int n = m->packages[i].value;
        int number_of_created = semctl(semaphor_set, CREATED_INDEX, GETVAL) + 1;
        int number_of_packed = semctl(semaphor_set, PACKED_INDEX, GETVAL);

        printf("(%d %lu) Przygotowalem zamowienie o wielkosci %d. ", getpid(), time(NULL), n);
        printf("Liczba paczek do przygotowania: %d. ", number_of_created);
        printf("Liczba paczek do wyslania: %d\n", number_of_packed);

        semop(semaphor_set, ops_end, 2);
        shmdt(m);

        sleep(2);
    }
}