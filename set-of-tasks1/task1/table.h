struct Table {
    char ***mainTable;
    int mainTableLength;
    int *operationsBlockLength;
};

char *concat(const char *s1, const char *s2);
void compareTwoFiles(char *fileName1, char *fileName2);
int getNumberOfOperations();
char **initializeBlockOfEditingOperations();
struct Table initializeTable();
int addOperationsToTable(struct Table *table);
int getInformationsAboutBlock(struct Table *table, int index);
void deleteBlock(struct Table *table, int index);
void deleteOperation(struct Table *table, int mainIndex, int blockIndex);