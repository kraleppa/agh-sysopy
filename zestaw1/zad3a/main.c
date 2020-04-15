#include <stdio.h>
#include <curses.h>
#include "table.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>


void compare_pairs(char *pairs){
    compareSequence(pairs);
}

void save_tmp_file(struct Table *table){
    addOperationsToTable(table);
}

void remove_block(struct Table *table, int index){
    deleteBlock(table, index);
}

void remove_operation(struct Table *table, int blockIndex, int operationIndex){
    deleteOperation(table, blockIndex, operationIndex);
}

void show_table(struct Table table){
    showAllTable(table);
}

void read_from_command_line(int argc, char *argv[]){
    struct Table table;
    table = initializeTable();

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "compare_pairs") == 0){
            char *pairsSeq = "";
            i++;
            while (i < argc && strcmp(argv[i], "remove_block") != 0 && strcmp(argv[i], "remove_operation") != 0 && strcmp(argv[i], "save_tmp_file") != 0 && strcmp(argv[i], "show_table") != 0){
                if (strcmp(argv[i - 1], "compare_pairs") != 0){
                    pairsSeq = concat(pairsSeq, " ");
                }
                pairsSeq = concat(pairsSeq, argv[i]);
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
    read_from_command_line(argc, argv); 
}