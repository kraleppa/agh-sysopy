#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
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

sem_t* created;
sem_t* space;
sem_t* can_modify;
sem_t* packed;
int mem;

int main(){
    created = sem_open("/created", O_RDWR, 0666);
    space = sem_open("/space", O_RDWR, 0666);
    can_modify = sem_open("/can_modify", O_RDWR, 0666);
    packed = sem_open("/packed", O_RDWR, 0666);
    mem = shm_open("/memory", O_RDWR, 0666);

    while(1){
        sem_wait(space);
        sem_wait(can_modify);
        int number = rand() % MAX_CREATED_COUNT + 1;
        memory_t* m = mmap(NULL, sizeof(memory_t), PROT_WRITE, MAP_SHARED, mem, 0);
        int i;
        if (m -> index){
            m -> index = 0;
            i = 0;
        } else 
            i = (m -> index + m -> size) % PACKAGES_COUNT;
        m -> packages[i].status = CREATED;
        m -> packages[i].value = number;
        m -> size++;
        int number_of_created;
        sem_getvalue(created, &number_of_created);
        
        int number_of_packed;
        sem_getvalue(packed, &number_of_packed);

        printf("(%d %lu) Dostalem liczbe %d.", getpid(), time(NULL), number);
        printf("Liczba paczek do przygotowania: %d.", number_of_created);
        printf("Liczba paczek do wyslania: %d \n", number_of_packed);        
        sem_post(created);
        sem_post(can_modify);        
        munmap(m, sizeof(memory_t));

        sleep(2);
    }
}