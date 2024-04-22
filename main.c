#include <stdio.h>
#include <math.h>

// Define color codes/text placeholders
#define BOARD_BG "\e[48;5;255m"
#define EMPTY_PIECE "\e[1;38;2;200;200;240m"
#define FULL_PIECE "\e[1;38;2;50;50;240m"
#define GAME_END "\e[4;1;38;2;152;195;121m"
#define NIM "\e[1;4;38;5;166mN\e[1;38;5;142mi\e[1;38;5;117mm\e[0;1;38;2;255;255;255m"
#define PLAYER "\e[1;91m"
#define RESET "\e[0;1;38;2;255;255;255m"
#define ROW_LABEL "\e[1;38;5;161m"
#define SPACER "-------------------------------------------------------------------------\n"


int gameWon(int row0[3], int row1[5], int row2[7]);

int getMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pickedRowFlag, int *pieces, int *saveFlag);

int legalMove(int row0[3], int row1[5], int row2[7], int chosenRow, int pieces);

int nimRowSum(int row0[3], int row1[5], int row2[7]);

int nimSum(int a, int b);

int readGame(int row0[3], int row1[5], int row2[7], int *player); // TODO: implement game reading

int rowSum(int row[], int size);

int writeGame(int row0[3], int row1[5], int row2[7], int player);

void displayBoard(int row0[3], int row1[5], int row2[7]);

void printRow(int row[], int number, int size);

void readRow(FILE *file, int row[]);

void removePieces(int row[], int pieces);

void writeRow(FILE *file, int row[], int size);

int main(void) {
    int row0[] = {1, 1, 1};
    int row1[] = {1, 1, 1, 1, 1};
    int row2[] = {1, 1, 1, 1, 1, 1, 1};
    int *row;
    int player = 0; // 0 = A; 1 = B
    int chosenRow;
    int pieces;
    int saveGameFlag = 0;
    int pickedRowFlag;
    int gameSelection;
    int readyFlag = 0;

    // TODO: Implement compete against CPU feature

    printf(RESET"Welcome to "NIM"!\n");
    while (!readyFlag) {
        printf("What would you like to do?\n1: Start new game\n2: Load game from file\n3: Start new game against computer\nEnter selection: ");
        scanf("%d", &gameSelection);
        if (gameSelection == 1) {
            printf("Ok. Preparing new game...\n");
            readyFlag = 1;
        } else if (gameSelection == 2) {
            readyFlag = readGame(row0, row1, row2, &player);
        } else if (gameSelection == 3) {
            // TODO: set up AI
        } else {
            printf("Invalid option. Type a number between 1 and 2");
        }
    }
    while (!gameWon(row0, row1, row2) && !saveGameFlag) {
        printf(SPACER);
        displayBoard(row0, row1, row2);
        printf("It is player "PLAYER"%c"RESET"'s turn.\n", player ? 'B' : 'A');
        printf("What move would you like to make?\n");
        pickedRowFlag = 0;

        while (!getMove(row0, row1, row2, &chosenRow, &pickedRowFlag, &pieces, &saveGameFlag)) {}
        if (saveGameFlag) {
            if (!writeGame(row0, row1, row2, player)) {
                saveGameFlag = 0;
                continue;
            }
        } else {
            row = chosenRow == 1 ? row0
                                 : (chosenRow == 2 ? row1
                                                   : row2);
            removePieces(row, pieces);
            player = !player;
        }
    }

    if (!saveGameFlag)
        printf(SPACER GAME_END"Player "PLAYER"%c"GAME_END" took the last piece.\nPlayer "PLAYER"%c"GAME_END" wins!",
               player ? 'A' : 'B', player ? 'B' : 'A');


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

int gameWon(int row0[3], int row1[5], int row2[7]) {
    int sum =
            rowSum(row0, 3) +
            rowSum(row1, 5) +
            rowSum(row2, 7);
    return sum == 0;
}

int getMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pickedRowFlag, int *pieces, int *saveFlag) {
    if (!*pickedRowFlag) {
        printf("Enter the row you would like to take from: ");
        scanf("%d", chosenRow);
        if (*chosenRow < 0) {
            *saveFlag = 1;
            return 1;
        }
        if (!legalMove(row0, row1, row2, *chosenRow, 1)) {
            printf("Invalid row!\n");
            return 0;
        }
        *pickedRowFlag = 1;
    }
    printf("Enter the number of pieces you would like to take from row %d: ", *chosenRow);
    scanf("%d", pieces);
    if (!legalMove(row0, row1, row2, *chosenRow, *pieces)) {
        printf("Invalid move!\n");
        return 0;
    }
    return 1;
}

int nimRowSum(int row0[3], int row1[5], int row2[7]) {
    int a = rowSum(row0, 3);
    int b = rowSum(row1, 5);
    int c = rowSum(row2, 7);

    return nimSum(nimSum(a, b), c);
}

int nimSum(int a, int b) {
    int c = 0;
    int i;

    for (i = 0; i < 3; i++) { // The largest sum possible is 7, which is represented in 3 bits
        c += (a % 2 ^ b % 2) * (int) pow(2, i);
        a >>= 1;
        b >>= 1;
    }

    return c;
}

int readGame(int row0[3], int row1[5], int row2[7], int *player) {
    FILE *file;
    char fileName[31];
    char playerChar;
    printf(SPACER);
    printf("Reading Game from File\n");
    printf("Enter the name of the file you would like to load the game from (30 character limit): ");
    scanf("%30s", fileName);
    file = fopen(fileName, "r");

    if (file == NULL) {
        printf("There is no file called %s. Returning to main menu...\n", fileName);
        printf(SPACER);
        return 0;
    }

    readRow(file, row0);
    readRow(file, row1);
    readRow(file, row2);
    fscanf(file, "%c", &playerChar);
    *player = playerChar == 'B';

    fclose(file);
    return 1;
}

int rowSum(int row[], int size) {
    int sum = 0;
    int i;
    for (i = 0; i < size; i++)
        sum += row[i];
    return sum;
}

int writeGame(int row0[3], int row1[5], int row2[7], int player) {
    FILE *file;
    char fileName[31];
    char createNewFile;
    printf(SPACER);
    printf("Saving Game to File\n");
    printf("Enter the name of the file you would like to save this game to (this will overwrite existing files) (30 character limit): ");
    scanf("%30s", fileName);
    file = fopen(fileName, "r");

    if (file == NULL) {
        printf("There is no file called %s. Do you want to create it? (Y/n) ", fileName);
        scanf(" %c", &createNewFile);
        if (createNewFile != 'y' && createNewFile != 'Y') {
            printf("Returning to game...\n");
            return 0;
        }
    } else {
        fclose(file);
    }

    file = fopen(fileName, "w");

    writeRow(file, row0, 3);
    writeRow(file, row1, 5);
    writeRow(file, row2, 7);
    fprintf(file, "%c.", player ? 'B' : 'A');

    fclose(file);
    printf("Game saved. Exiting...");
    return 1;
}

void displayBoard(int row0[3], int row1[5], int row2[7]) {
    printRow(row0, 1, 3);
    printRow(row1, 2, 5);
    printRow(row2, 3, 7);
}

void printRow(int *row, int number, int size) {
    int i;
    printf(ROW_LABEL"Row %d%*c"RESET, number, 10 - size, ' ');
    for (i = 0; i < size; i++) {
        if (*(row + i))
            printf(BOARD_BG" "FULL_PIECE"■"RESET);
        else
            printf(BOARD_BG" "EMPTY_PIECE"■"RESET);
    }
    printf(BOARD_BG" "RESET"\n");
}

void readRow(FILE *file, int row[]) {
    char c = ' ';
    int i = 0;
    while (c != '.') {
        fscanf(file, "%c", &c);
        if (c == ',')
            continue;
        else if (c == 'E')
            row[i] = 0;
        else if (c == 'F')
            row[i] = 1;
        i++;
    }
    fseek(file, sizeof(char), SEEK_CUR);
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

void writeRow(FILE *file, int row[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        fprintf(file, "%c", row[i] ? 'F' : 'E');
        fprintf(file, "%c", i == size - 1 ? '.' : ',');
    }
    fprintf(file, "\n");
}
