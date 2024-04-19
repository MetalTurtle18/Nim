#include <stdio.h>

int legalMove(int row0[3], int row1[5], int row2[7], int chosenRow, int pieces);

int getMove(int row0[], int row1[], int row2[], int *chosenRow, int *pieces, int *saveFlag);

int gameWon(int row0[3], int row1[5], int row2[7]);

int rowSum(int row[], int size);

int readGame(int row0[3], int row1[5], int row2[7], int *player);

void writeGame(int row0[3], int row1[5], int row2[7], int player);

void writeRow(FILE *file, int row[], int size);

void removePieces(int row[], int pieces);

void displayBoard(int row0[3], int row1[5], int row2[7]);

void printRow(int row[], int number, int size);

int main(void) {
    int row0[] = {1, 1, 1};
    int row1[] = {1, 1, 1, 1, 1};
    int row2[] = {1, 1, 1, 1, 1, 1, 1};
    int *row;
    int player = 0; // 0 = A; 1 = B
    int chosenRow;
    int pieces;
    int saveGameFlag = 0;

    while (!gameWon(row0, row1, row2) && !saveGameFlag) {
        printf("---------------------------------------------\n");
        displayBoard(row0, row1, row2);
        printf("It is player %c's turn.\n", player ? 'B' : 'A');

        while (!getMove(row0, row1, row2, &chosenRow, &pieces, &saveGameFlag)) {}
        if (saveGameFlag) {
            writeGame(row0, row1, row2, player);
        } else {
            row = chosenRow == 1 ? row0
                                 : (chosenRow == 2 ? row1
                                                   : row2);
            removePieces(row, pieces);
            player = !player;
        }
    }

    if (!saveGameFlag)
        printf("Player %c wins!", player ? 'B' : 'A');

    return 0;
}

int legalMove(int row0[3], int row1[5], int row2[7], int row, int pieces) {
    if (pieces > 3 || pieces < 0 || row > 3 || row < 0)
        return 0;
    switch (row) {
        case 1:
            return rowSum(row0, 3) >= pieces;
        case 2:
            return rowSum(row1, 5) >= pieces;
        default:
            return rowSum(row2, 7) >= pieces;
    }
}

int getMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pieces, int *saveFlag) {
    printf("What move would you like to make (enter in the form 'row pieces' eg. '2 3')? "); // TODO: split row and pieces
    scanf("%d %d", chosenRow, pieces);
    if (*chosenRow < 0) {
        *saveFlag = 1;
        return 1;
    }
    if (!legalMove(row0, row1, row2, *chosenRow, *pieces)) {
        printf("Invalid move!\n");
        return 0;
    }
    return 1;
}

int gameWon(int row0[3], int row1[5], int row2[7]) {
    int sum =
            rowSum(row0, 3) +
            rowSum(row1, 5) +
            rowSum(row2, 7);
    return sum == 0;
}

int rowSum(int row[], int size) {
    int sum = 0;
    int i;
    for (i = 0; i < size; i++)
        sum += row[i];
    return sum;
}

void writeGame(int row0[3], int row1[5], int row2[7], int player) {
    FILE *file;
    char fileName[31];
    printf("---------------------------------------------\n");
    printf("Saving Game to File\n");
    printf("Enter the name of the file you would like to save this game to (this will overwrite existing files) (30 character limit): ");
    scanf("%30s", fileName);
    file = fopen(fileName, "w"); // TODO: handle new files

    writeRow(file, row0, 3);
    writeRow(file, row1, 5);
    writeRow(file, row2, 7);
    fprintf(file, "%c.", player ? 'B' : 'A');

    fclose(file);
}

void writeRow(FILE *file, int row[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        fprintf(file, "%c", row[i] ? 'F' : 'E');
        fprintf(file, "%c", i == size -1 ? '.' : ',');
    }
    fprintf(file, "\n");
}

void removePieces(int row[], int pieces) {
    int removed = 0;
    int i = 0;
    while (removed < pieces) {
        removed += row[i];
        row[i] = 0;
        i++;
    }
}

void displayBoard(int row0[3], int row1[5], int row2[7]) {
    printRow(row0, 1, 3);
    printRow(row1, 2, 5);
    printRow(row2, 3, 7);
}

void printRow(int *row, int number, int size) {
    int i;
    printf("Row %d%*c", number, 10 - size, ' ');
    for (i = 0; i < size; i++)
        printf("%2c", *(row + i) ? '|' : ' ');
    printf("\n");
}