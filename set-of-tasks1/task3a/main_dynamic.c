#include <stdio.h>
#include <curses.h>
#include "table.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <dlfcn.h>

void *dl_handle;

typedef void *(*arbitrary)();

char *concat_(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void compare_pairs(char *pairs){
    arbitrary fcompareSequence;
    *(void **)(&fcompareSequence) = dlsym(dl_handle, "compareSequence");
    fcompareSequence(pairs);
}

void save_tmp_file(struct Table *table){
    arbitrary faddOperationsToTable;
    *(void **)(&faddOperationsToTable) = dlsym(dl_handle, "addOperationsToTable");
    faddOperationsToTable(table);
}

void remove_block(struct Table *table, int index){
    arbitrary fdeleteBlock;
    *(void **)(&fdeleteBlock) = dlsym(dl_handle, "deleteBlock");
    fdeleteBlock(table, index);
}

void remove_operation(struct Table *table, int blockIndex, int operationIndex){
    arbitrary fdeleteOperation;
    *(void **)(&fdeleteOperation) = dlsym(dl_handle, "deleteOperation");
    fdeleteOperation(table, blockIndex, operationIndex);
}

void show_table(struct Table table){
    arbitrary fshowAllTable;
    *(void **)(&fshowAllTable) = dlsym(dl_handle, "showAllTable");
    fshowAllTable(table);
}

void read_from_command_line(int argc, char *argv[]){
    struct Table table;
    table.mainTableLength = 0;
    table.operationsBlockLength = NULL;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "compare_pairs") == 0){
            char *pairsSeq = "";
            i++;
            while (i < argc && strcmp(argv[i], "remove_block") != 0 && strcmp(argv[i], "remove_operation") != 0 && strcmp(argv[i], "save_tmp_file") != 0 && strcmp(argv[i], "show_table") != 0){
                if (strcmp(argv[i - 1], "compare_pairs") != 0){
                    pairsSeq = concat_(pairsSeq, " ");
                }
                pairsSeq = concat_(pairsSeq, argv[i]);
                i++;
            }
            compare_pairs(pairsSeq);
            continue;
        }

        if (strcmp(argv[i], "remove_block") == 0){
            remove_block(&table, (int)(argv[i + 1][0] - '0'));
            i++;
            continue;
            
        }

        if (strcmp(argv[i], "remove_operation") == 0){
            remove_operation(&table, (int)(argv[i + 1][0] - '0'), (int)(argv[i + 2][0] - '0'));
            i += 2;
            continue;
        }

        if (strcmp(argv[i], "save_tmp_file") == 0){
            save_tmp_file(&table);
            continue;
        }

        if (strcmp(argv[i], "show_table") == 0){
            show_table(table);
            continue;
        }
    }
}



int main(int argc, char *argv[])
{
    dl_handle = dlopen("./libtable.so", RTLD_LAZY);
    if (!dl_handle) {
        printf("!!! %s\n", dlerror());
        return 0;
    }
    read_from_command_line(argc, argv); 
}