#include <stdio.h>
#include "table.h"

int main(){
    struct Table tab = initializeTable(1);
    addOperationsToTable(&tab);
    addOperationsToTable(&tab);
    addOperationsToTable(&tab);
    deleteBlock(&tab, 1);
    showAllTable(tab);
}