#include <stdio.h>
#include <curses.h>
#include "table.h"
#include <string.h>
#include <stdlib.h>

struct Table newTable(){
    struct Table table = initializeTable();
    return table;
}

void addComparesToTable(struct Table *table, char **pairs, int size){     //pairs[x] = "file1A.txt:file1B.txt"
    char korektor[] = ":";
    for (int i = 0; i < size; i++){
        //zamieniam pairs na statyczne
        char nowa[strlen(pairs[i])];
        char *pairsCopy = pairs[i];
        for (int j = 0; j < strlen(pairs[i]); j++){
            nowa[j] = pairsCopy[j];
        }
        //dziele sekwencje na 2 pliki
        char *token = strtok(nowa, korektor);
        char *plik1 = token;
        token = strtok(NULL, korektor);
        char *plik2 = token;

        compareTwoFiles(plik1, plik2);
        addOperationsToTable(table);
    }
}

void removeBlock(struct Table *table, int index){
    deleteBlock(table, index);
}

void removeOperation(struct Table *table, int blockIndex, int operationIndex){
    deleteOperation(table, blockIndex, operationIndex);
}



int main(int argc, char *argv[])
{
    char **pairs = calloc(2, sizeof(char*));
    pairs[0] = "a.txt:b.txt";
    pairs[1] = "a.txt:c.txt";

    struct Table table;
    table = newTable();
    addComparesToTable(&table, pairs, 2);
    removeBlock(&table, 1);
    showAllTable(table);
    removeOperation(&table, 0, 1);
    showAllTable(table);
}