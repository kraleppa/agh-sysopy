#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COLumnS_NUMBER 1000
#define MAX_LINE_LENGTH (MAX_COLumnS_NUMBER * 5)


typedef struct {
    int **values;
    int rows;
    int columns;
} Matrix;

int countColumns(char *row){
    int number = 0;
    char *element = strtok(row, " ");
    while (element != NULL){
        if (strcmp(element, "\n") != 0){
            number++;
        }
        element = strtok(NULL, " ");
    }
    return number;
}

void setMatrixSize(FILE *file, int *rows, int *columns){
    char line[MAX_LINE_LENGTH];
    *columns = 0;
    *rows = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        if (*columns == 0){
            *columns = countColumns(line);
        }
        *rows = *rows + 1;
    }

    fseek(file, 0, SEEK_SET);
}

Matrix *initMatrix(char *path){
    FILE *file = fopen(path, "r");

    int rows, columns;
    setMatrixSize(file, &rows, &columns);


    int **values = calloc(rows, sizeof(int*));

    for (int i = 0; i < rows; i++){
        values[i] = calloc(columns, sizeof(int));
    }

    int x = 0;
    int y = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        x = 0;
        char *number = strtok(line, " ");
        while (number != NULL){
            values[y][x] = atoi(number);
            x++;
            number = strtok(NULL, " ");
        }
        y++;
    }

    fclose(file);

    Matrix *matrix = calloc(1, sizeof(Matrix));
    matrix -> columns = columns;
    matrix -> rows = rows;
    matrix -> values = values;

    return matrix;
}

void deleteMatrix(Matrix *matrix){
    for (int i = 0; i < matrix -> rows; i++){
        free(matrix -> values[i]);
    }
    free(matrix -> values);
}

void printMatrix(Matrix *matrix){
    for (int i = 0; i < matrix -> rows; i++){
        for (int j= 0; j < matrix -> columns; j++){
            printf("%d ", matrix -> values[i][j]);
        }
        printf("\n");
    }
}

int main(){
    Matrix *matrix = initMatrix("matrix");
    
    printMatrix(matrix);
    deleteMatrix(matrix);
    return 0;
}