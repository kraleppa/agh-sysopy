#include <stdio.h>
#include "table.h"

int main(){
    struct Table tab = initializeTable();
    printf("%d\n", tab.mainTableLength);
}