#include <stdio.h>
#include <stdlib.h> 

int *createTable(int size){
    int *table = (int*) malloc (size * sizeof(int));
    for (int i = 0; i < size; i++){
        table[i] = i;
    }
    return table;
}

void printTable(int *table, int size){
    for (int i = 0; i < size; i++){
        printf("%d\n", table[i]);
    }
}

void delteTable(int *table){
    free(table);
}


int main(){
    printf("Hello World\n");
    int *table = createTable(8);
    printTable(table, 8);
    //free(table);
    return 0;
}