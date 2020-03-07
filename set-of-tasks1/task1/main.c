#include <stdio.h>
#include "table.h"

int main(){
    struct Table tab = initializeTable(1);
    addOperationsToTable(&tab);
    printf("%s\n", tab.mainTable[0][0]);
}