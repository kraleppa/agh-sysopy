#include <stdio.h>
#include "table.h"

int main(){
    compareSequence("a.txt:b.txt b.txt:a.txt");
    struct Table tab = initializeTable();
    int x = addOperationsToTable(&tab);
    showAllTable(tab);
    printf("%d\n", x);
    return 0;
}