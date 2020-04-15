#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h> 
#include <time.h>

#define MAX_COLUMNS_NUMBER 1000
#define MAX_LINE_LENGTH (MAX_COLUMNS_NUMBER * 5)

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
typedef struct {
    int **values;
    int rows;
    int columns;
} Matrix;


void deleteMatrix(Matrix *matrix){
    for (int i = 0; i < matrix -> rows; i++){
        free(matrix -> values[i]);
    }
    free(matrix -> values);
}


void generate_matrix(int rows, int cols, char* filename) {
    FILE* file = fopen(filename, "w+");
   
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (x > 0) {
                fprintf(file, " ");
            }
            
            fprintf(file, "%d", rand() % (200 + 1) - 100);
            
        };
        fprintf(file, "\n");
    }
    fclose(file);
}


Matrix *multiply_matrices(Matrix *a, Matrix *b){
    
    int **values = calloc(a -> rows, sizeof(int*));
    for (int y = 0; y < a -> rows; y++) {
        values[y] = calloc(b -> columns, sizeof(int));
    }

    for (int i = 0; i < a -> rows; i++) {
        for (int j = 0; j < b -> columns; j++) {
            int result = 0;
            for(int k = 0; k < a -> columns; k++){
                result += (a -> values[i][k] * b -> values[k][j]);
            }
            values[i][j] = result;
        }
    }
   

    Matrix *new_matrix = calloc(1, sizeof(Matrix));
    new_matrix -> values = values;
    new_matrix -> rows = a -> rows;
    new_matrix -> columns = b -> columns;

    return new_matrix;
}

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

bool check_multiply_correctness(char *a_filename, char *b_filename, char *c_filename){
    Matrix *a = initMatrix(a_filename);
    Matrix *b = initMatrix(b_filename);
    Matrix *c = initMatrix(c_filename);
    Matrix *correct_matrix = multiply_matrices(a, b);
    if(correct_matrix -> columns != c -> columns || correct_matrix -> rows != c -> rows) return false;
    for( int i = 0; i < correct_matrix -> rows; i++){
        for(int j = 0; j<correct_matrix -> columns; j++){
            if(correct_matrix -> values[i][j] != c -> values[i][j]) return false;
        }
    }
    deleteMatrix(a);
    deleteMatrix(b);
    deleteMatrix(c);
    deleteMatrix(correct_matrix);
    return true;
}


int main(int argc, char *argv[]){
    if (argc != 5){
        perror("syntax error");
    }

    srand(time(NULL));
    char *mode = argv[1];

    if (strcmp(mode, "create") == 0){
        int min = atoi(argv[2]);
        int max = atoi(argv[3]);
        int numberOfMatrices = atoi(argv[4]);
        system("rm lista");
        system("touch lista");
        system("rm -rf matrix");
        system("mkdir matrix");

        for (int i = 0; i < numberOfMatrices; i++){
            int rows1 = rand() % (max - min + 1) + min;;
            int columns1 = rand() % (max - min + 1) + min;
            int columns2 = rand() % (max - min + 1) + min;
            char *name1 = calloc(100, sizeof(char));
            char *name2 = calloc(100, sizeof(char));
            char *name3 = calloc(100, sizeof(char));
            sprintf(name1, "matrix/a%d", i);
            sprintf(name2, "matrix/b%d", i);
            sprintf(name3, "matrix/c%d", i);
            generate_matrix(rows1, columns1, name1);
            generate_matrix(columns1, columns2, name2);
            system(concat("touch ", name3));

            
            char *command = calloc(1000, sizeof(char));
            sprintf(command, "echo \"%s %s %s\" >> lista", name1, name2, name3);
            system(command);
        }
    } else if(strcmp(mode, "test") == 0){
        char a_filename[PATH_MAX + 1];
        char b_filename[PATH_MAX + 1];
        char c_filename[PATH_MAX + 1];
        strcpy(a_filename, argv[2]);
        strcpy(b_filename, argv[3]);
        strcpy(c_filename, argv[4]);
        if (!check_multiply_correctness(a_filename, b_filename, c_filename)){
            printf("%s     %s     ok\n", a_filename, b_filename);
        }else{
            printf("%s     %s     error\n", a_filename, b_filename);
        }
    }
    return 0;

}