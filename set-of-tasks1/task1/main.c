#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>

//connects two strings
char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

//compares two files and saves the result to tmp.txt
void compareTwoFiles(char *fileName1, char *fileName2){
    char *command = concat("diff ", concat(fileName1, concat(" ", concat(fileName2, " > tmp.txt"))));
    system(command);
}

int getNumberOfOperations(){
    FILE *filePoiner = fopen("tmp.txt", "r");
    if (filePoiner == NULL){
        exit(EXIT_FAILURE);
    }

    char buffer[255];
    int numberOfOperations = 0;

    while (fgets(buffer, 255, filePoiner)){
        if (isdigit(buffer[0])){
            numberOfOperations++;
        }  
    }
    return numberOfOperations;
}

void initializeBlockOfEditingOperations(){
    FILE *filePoiner = fopen("tmp.txt", "r");
    char buffer[255];
    if (filePoiner == NULL){
        exit(EXIT_FAILURE);
    }
    int numberOfOperations = getNumberOfOperations();
    char **block = calloc(numberOfOperations, sizeof(char*));
    char *operation = "null";
    int i = 0;
    while (fgets(buffer, 255, filePoiner)){
        if (isdigit(buffer[0])){
            if (i != 0){
                block[i - 1] = operation;
            }
            operation = "";
            operation = concat(operation, buffer);

            i++;
            
        } else {
            operation = concat(operation, buffer);
        }
    }
    block[i - 1] = operation;
    printf("Value at bp1: \n%s", block[0]);
    printf("Value at bp2: \n%s", block[1]);
    printf("Value at bp3: \n%s", block[2]);

}


int main(){
    initializeBlockOfEditingOperations();
}