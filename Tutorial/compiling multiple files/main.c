#include <stdio.h>
#include "functions.h"

int main(){
    start();
    int number = 5;
    printf("result: %d\n", squareRoot(number));
    end();
    return 0;
}