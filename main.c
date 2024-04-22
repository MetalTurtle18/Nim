#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define color codes/text placeholders
#define BOARD_BG "\e[48;5;255m"
#define EMPTY_PIECE "\e[1;38;2;200;200;240m"
#define FULL_PIECE "\e[1;38;2;50;50;240m"
#define GAME_END "\e[4;1;38;2;152;195;121m"
#define NIM "\e[1;4;38;5;166mN\e[1;38;5;142mi\e[1;38;5;117mm\e[0;1;38;2;255;255;255m"
#define PIECES "\e[1;95m"
#define PLAYER "\e[1;91m"
#define RESET "\e[0;1;38;2;255;255;255m"
#define ROW_LABEL "\e[1;38;5;161m"
#define SPACER "-------------------------------------------------------------------------\n"

/**
 * Determine if the game is over
 * @param row0
 * @param row1
 * @param row2
 * @return 1 if the game is over, 0 if not
 */
int gameWon(int row0[3], int row1[5], int row2[7]);

/**
 * Prompt the user for their next move
 * @param row0
 * @param row1
 * @param row2
 * @param chosenRow address to store the chosen row
 * @param pickedRowFlag address for flag for whether the user has successfully picked a row
 * @param pieces address to store the chosen number of pieces to take
 * @param saveFlag address for flag for whether the user has opted to save the game at this point
 * @return 1 if the move has been chosen successfully or if the user chose to save the game, 0 otherwise
 */
int getMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pickedRowFlag, int *pieces, int *saveFlag);

/**
 * Determine whether a given move is legal
 * @param row0
 * @param row1
 * @param row2
 * @param chosenRow
 * @param pieces
 * @return 1 if the move is legal, 0 if not
 */
int legalMove(int row0[3], int row1[5], int row2[7], int chosenRow, int pieces);

/**
 * Calculate the nim sum of three given rows
 * @param row0
 * @param row1
 * @param row2
 * @return the nim sum
 */
int nimRowSum(int row0[3], int row1[5], int row2[7]);

/**
 * calculate the nim sum of two numbers
 * @param a
 * @param b
 * @return the nim sum
 */
int nimSum(int a, int b);

/**
 * Read a game from a file
 * @param row0
 * @param row1
 * @param row2
 * @param player address to store the player who is up next read from the file
 * @return 1 if the file was read successfully, 0 otherwise
 */
int readGame(int row0[3], int row1[5], int row2[7], int *player);

/**
 * Count the number of pieces left in a row
 * @param row
 * @param size the total number of pieces that can fit in this row
 * @return the number of pieces left in the row
 */
int rowSum(int row[], int size);

/**
 * Write the game to a file
 * @param row0
 * @param row1
 * @param row2
 * @param player the player who is up next
 * @return 1 if the game was written successfully, 0 otherwise
 */
int writeGame(int row0[3], int row1[5], int row2[7], int player);

/**
 * Pretty print the game board
 * @param row0
 * @param row1
 * @param row2
 */
void displayBoard(int row0[3], int row1[5], int row2[7]);

/**
 * Get the next best legal move in the game
 * @param row0
 * @param row1
 * @param row2
 * @param chosenRow address to store the chosen row
 * @param pieces address to store the chosen number of pieces to take
 */
void getAIMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pieces);

/**
 * Pretty print a single row
 * @param row
 * @param number human-friendly index of the row (1, 2, 3)
 * @param size the total number of items that fit in this row
 */
void printRow(int row[], int number, int size);

/**
 * Read a single row from a file
 * @param file address of the file data
 * @param row the row to save data into
 */
void readRow(FILE *file, int row[]);

/**
 * Remove the given number of pieces from the given row
 * @param row
 * @param pieces
 */
void removePieces(int row[], int pieces);

/**
 * Prompt the user for options on how they can play against the computer
 * @param aiPlayer address of the computer's player number (0 or 1)
 */
void setUpAI(int *aiPlayer);

/**
 * Prompt the user for options on how they can play the game
 * @param row0
 * @param row1
 * @param row2
 * @param player address of the player who is up (0 or 1)
 * @param computerGame address of boolean for whether this game is against the computer
 * @param aiPlayer address of the computer's player number (0 or 1)
 */
void setUpGame(int row0[3], int row1[5], int row2[7], int *player, int *computerGame, int *aiPlayer);

/**
 * Write a single row to a file
 * @param file address of the file data
 * @param row the row to write
 * @param size the number of items that can fit in the row
 */
void writeRow(FILE *file, int row[], int size);

int main(void) {
    // Arrays for each row
    int row0[] = {1, 1, 1};
    int row1[] = {1, 1, 1, 1, 1};
    int row2[] = {1, 1, 1, 1, 1, 1, 1};
    int *row; // Pointer for passing individual rows to functions
    int aiPlayer; // Which turn the AI player gets
    int chosenRow; // Which row the player chose to take from
    int pickedRowFlag; // Tracks whether the player has successfully picked a row
    int pieces; // How many pieces the player chose to take
    int computerGame = 0; // Whether the player is playing against the computer
    int player = 0; // Which player's turn it is; 0 = A; 1 = B
    int saveGameFlag = 0; // Tracks whether the player chose to save the game on their most recent input

    printf(RESET"Welcome to "NIM"!\n");

    setUpGame(row0, row1, row2, &player, &computerGame, &aiPlayer);

    // Loop as long as the game is not won and there user did not choose to save the game
    while (!gameWon(row0, row1, row2) && !saveGameFlag) {
        printf(SPACER);
        displayBoard(row0, row1, row2);
        printf("It is player "PLAYER"%c"RESET"'s turn.\n", player ? 'B' : 'A');

        // This condition ensures the program does not ask for a move if it is the computer's turn
        if (!computerGame || player != aiPlayer) {
            printf("What move would you like to make?\n");
            pickedRowFlag = 0;

            // As long as the move is not successful, continue asking
            while (!getMove(row0, row1, row2, &chosenRow, &pickedRowFlag, &pieces, &saveGameFlag)) {/* none */}
        } else { // Otherwise, it must be the computer's turn to choose a move
            getAIMove(row0, row1, row2, &chosenRow, &pieces);
        }

        if (saveGameFlag) { // If the user chose to save the game in their last move
            // This condition calls the function to save the game. If it FAILS, then the condition will pass
            if (!writeGame(row0, row1, row2, player)) {
                saveGameFlag = 0;
                continue; // Since saving failed, return to the game that was going on
            }
        } else { // If we got here and the user did not try to save, then they successfully chose a move.
            printf("Player "PLAYER"%c"RESET" will remove "PIECES"%d"RESET" pieces from "ROW_LABEL"Row %d"RESET"\n",
                   player ? 'B' : 'A', pieces, chosenRow);

            // Translate the rows from 1 indexed (user-facing) to the real variable names
            if (chosenRow == 1)
                row = row0;
            else if (chosenRow == 2)
                row = row1;
            else
                row = row2;

            removePieces(row, pieces); // Execute the move

            player = !player; // Switch which player's turn it is
        }
    }

    if (!saveGameFlag) // The game loop can exit if the user saves or if the game ends; only print if game is over.
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

void getAIMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pieces) {
    int X = nimRowSum(row0, row1, row2);
    int h1, h2, h3;
    int h1SumX, h2SumX, h3SumX;

    if (X == 0) { // This means we are not in a guaranteed winning position; make a dummy move to move the game along
        *pieces = 1;
        if (rowSum(row0, 3))
            *chosenRow = 1;
        else if (rowSum(row1, 5))
            *chosenRow = 2;
        else
            *chosenRow = 3;
        return;
    }
    // If we get here, the nim sum is nonzero. So there exists a move to make it zero

    h1 = rowSum(row0, 3);
    h1SumX = nimSum(X, h1);

    h2 = rowSum(row1, 5);
    h2SumX = nimSum(X, h2);

    h3 = rowSum(row2, 7);
    h3SumX = nimSum(X, h3);

    if (h1 + h2 + h3 == 1) { // Sorry buddy you lost
        *pieces = 1;
        if (h1 > 1)
            *chosenRow = 1;
        else if (h2 > 1)
            *chosenRow = 2;
        else
            *chosenRow = 3;
        return;
    }

    if (h1 == 1 && h2 == 1 && h3 < 4 || // There are two ones
        h1 == 1 && h3 == 1 && h2 < 4 || // There are two ones
        h2 == 1 && h3 == 1 && h1 < 4 || // There are two ones
        h1 == 0 && h2 == 0 && h3 < 5 || // There is only one heap with pieces
        h1 == 0 && h3 == 0 && h2 < 5 || // There is only one heap with pieces
        h2 == 0 && h3 == 0 && h1 < 5) { // There is only one heap with pieces
        if (h1 > 1) {
            *chosenRow = 1;
            *pieces = h1 - 1;
        } else if (h2 > 1) {
            *chosenRow = 2;
            *pieces = h2 - 1;
        } else {
            *chosenRow = 3;
            *pieces = h3 - 1;
        }
        return;
    }

    if (h1SumX < h1 && h1 - h1SumX <= 3) {
        *chosenRow = 1;
        *pieces = h1 - h1SumX;
    } else if (h2SumX < h2 && h2 - h2SumX <= 3) {
        *chosenRow = 2;
        *pieces = h2 - h2SumX;
    } else if (h3SumX < h3 && h3 - h3SumX <= 3) {
        *chosenRow = 3;
        *pieces = h3 - h3SumX;
    } else { // Backup move if there is no way to make the right move
        *pieces = 1;
        if (rowSum(row0, 3))
            *chosenRow = 1;
        else if (rowSum(row1, 5))
            *chosenRow = 2;
        else
            *chosenRow = 3;
    }
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

void setUpAI(int *aiPlayer) {
    int input;

    printf(SPACER);

    while (1) {
        printf("Who goes first?\n1: I go first\n2: Computer goes first\n3: Randomize\nEnter selection: ");
        scanf("%d", &input);
        if (input == 1) {
            printf("Ok. You are player "PLAYER"A"RESET".  Preparing new game...\n");
            *aiPlayer = 1;
        } else if (input == 2) {
            printf("Ok. You are player "PLAYER"B"RESET".  Preparing new game...\n");
            *aiPlayer = 0;
        } else if (input == 3) {
            *aiPlayer = rand() % 2;
            printf("Ok. You are player "PLAYER"%c"RESET".  Preparing new game...\n", *aiPlayer ? 'A' : 'B');
        } else {
            printf("Invalid option. Type a number between 1 and 3\n");
            continue;
        }
        break;
    }
}

void setUpGame(int row0[3], int row1[5], int row2[7], int *player, int *computerGame, int *aiPlayer) {
    int input;

    printf(SPACER);

    while (1) {
        printf("What would you like to do?\n1: Start new game\n2: Load game from file\n3: Start new game against computer\nEnter selection: ");
        scanf("%d", &input);
        if (input == 1) {
            printf("Ok. Preparing new game...\n");
        } else if (input == 2) {
            if (!readGame(row0, row1, row2, player))
                continue;
        } else if (input == 3) {
            printf("Ok. Preparing new game against computer...\n");
            *computerGame = 1;
            setUpAI(aiPlayer);
        } else {
            printf("Invalid option. Type a number between 1 and 3\n");
            continue;
        }
        break;
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
