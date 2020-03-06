#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

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


int main(){
    compareTwoFiles("a.txt", "b.txt");
}