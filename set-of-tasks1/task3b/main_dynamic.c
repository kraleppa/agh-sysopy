#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "table.h"

#include <dlfcn.h>


void *dl_handle;

typedef void *(*arbitrary)();

void addComparesToTable(struct Table *table, char **pairs, int size){     //pairs[x] = "file1A.txt:file1B.txt"
    clock_t start, stop;
    
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

        arbitrary fcompareTwoFiles;
        *(void **)(&fcompareTwoFiles) = dlsym(dl_handle, "compareTwoFiles");
        fcompareTwoFiles(plik1, plik2);
        start = clock();
        arbitrary faddOperationsToTable;
        *(void **)(&faddOperationsToTable) = dlsym(dl_handle, "addOperationsToTable");
        faddOperationsToTable(table);
        stop = clock();

        printf("%d argument: %f\n", i, (((double) (stop - start)) / CLOCKS_PER_SEC));
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
    
    arbitrary fcompareTwoFiles;
    *(void **)(&fcompareTwoFiles) = dlsym(dl_handle, "compareTwoFiles");
    fcompareTwoFiles(plik1, plik2);

    clock_t start, stop;
    start = clock();
    arbitrary faddOperationsToTable;
    *(void **)(&faddOperationsToTable) = dlsym(dl_handle, "addOperationsToTable");
    faddOperationsToTable(table);
    stop = clock();

    printf("added %s:%s time: %f\n", plik1 , plik2, (((double) (stop - start)) / CLOCKS_PER_SEC));
}

void removeBlock(struct Table *table, int index){
    clock_t start, stop;
    start = clock();
    arbitrary fdeleteBlock;
    *(void **)(&fdeleteBlock) = dlsym(dl_handle, "deleteBlock");   
    fdeleteBlock(table, index);
    stop = clock();
    printf("removed from %d time: %f\n", index, (((double) (stop - start)) / CLOCKS_PER_SEC));
}

void removeOperation(struct Table *table, int blockIndex, int operationIndex){
    arbitrary fdeleteOperation;
    *(void **)(&fdeleteOperation) = dlsym(dl_handle, "deleteOperation");  
    fdeleteOperation(table, blockIndex, operationIndex);
}

void readFromCommandLine(char *argv[], int argc){
    struct Table table;
    
    table.mainTableLength = 0;
    table.operationsBlockLength = NULL;
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
    //showAllTable(table);
}



int main(int argc, char *argv[])
{
    dl_handle = dlopen("./libtable.so", RTLD_LAZY);
    if (!dl_handle) {
        printf("!!! %s\n", dlerror());
        return 0;
    }

    readFromCommandLine(argv, argc);
}