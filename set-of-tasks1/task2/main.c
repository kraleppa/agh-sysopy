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

void addCompareToTable(struct Table *table, char *pair){
    char korektor[] = ":";
    char nowa[strlen(pair)];
    for (int j = 0; j < strlen(pair); j++){
            nowa[j] = pair[j];
    }
    char *token = strtok(nowa, korektor);
    char *plik1 = token;
    token = strtok(NULL, korektor);
    char *plik2 = token;
    compareTwoFiles(plik1, plik2);
    addOperationsToTable(table);
}

void removeBlock(struct Table *table, int index){
    deleteBlock(table, index);
}

void removeOperation(struct Table *table, int blockIndex, int operationIndex){
    deleteOperation(table, blockIndex, operationIndex);
}

void readFromCommandLine(char *argv[], int argc){
    struct Table table;
    table = initializeTable();
    bool filesStream = false;

    for (int i = 0; i < argc; i++){
        if (strcmp(argv[i], "compare_pairs") == 0){
            filesStream = true;
            continue;
        }

        if (strcmp(argv[i], "remove_block") == 0){
            filesStream = false;
            removeBlock(&table, (int)(argv[i + 1][0] - '0'));
            i++;
            continue;
            
        }

        if (strcmp(argv[i], "remove_operation") == 0){
            filesStream = false;
            removeOperation(&table, (int)(argv[i + 1][0] - '0'), (int)(argv[i + 2][0] - '0'));
            i += 2;
            continue;
        }

        if (filesStream){
            addCompareToTable(&table, argv[i]);
            continue;
        }
    }
    showAllTable(table);
}



int main(int argc, char *argv[])
{
    readFromCommandLine(argv, argc);
}