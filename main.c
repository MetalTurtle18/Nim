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
 * Calculate the nim sum of three given rows (same thing as XOR)
 * @param row0
 * @param row1
 * @param row2
 * @return the nim sum
 */
int nimSum(int row0[3], int row1[5], int row2[7]);

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
 * Make the first allowed move given the rows
 * @param row0
 * @param row1
 * @param row2
 * @param chosenRow address to store the chosen row
 * @param pieces address to store the chosen number of pieces to take
 */
void firstAvailableMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pieces);

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
    if (pieces > 3 || pieces < 0 || row > 3 || row < 0) // First check if the choices are in max bounds
        return 0;
    // Then check if there are enough pieces left in the chosen row
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
    return sum == 0; // The game is over if all pieces are gone. Simplest check is to sum all pieces.
}

int getMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pickedRowFlag, int *pieces, int *saveFlag) {
    if (!*pickedRowFlag) { // This check ensures the program does not prompt the user to enter the row again
        printf("Enter the row you would like to take from (or -1 to save the game): ");
        scanf("%d", chosenRow);
        if (*chosenRow < 0) { // If the user enters -1 (or any negative number), initiate game saving
            *saveFlag = 1;
            return 1; // Return 1 to break out of the game loop in the main function
        }
        if (!legalMove(row0, row1, row2, *chosenRow, 1)) { // If the move is invalid, tell the user
            printf("Invalid row!\n");
            return 0; // Go back to the main loop and return 0 so that it repeats.
        }
        // If the other conditions are not met, then the move is legal. Set flag to not ask the user for a row again
        *pickedRowFlag = 1;
    }
    // Now, get the number of pieces
    printf("Enter the number of pieces you would like to take from row %d: ", *chosenRow);
    scanf("%d", pieces);
    if (!legalMove(row0, row1, row2, *chosenRow, *pieces)) { // Similar invalid check as above
        printf("Invalid move!\n");
        return 0;
    }
    return 1; // If all is well, return 1 and move on
}

int nimSum(int row0[3], int row1[5], int row2[7]) {
    int a = rowSum(row0, 3);
    int b = rowSum(row1, 5);
    int c = rowSum(row2, 7);

    // To get the nim sum of three numbers, take the nim sum of two and then the sum of that with the third
    return (a ^ b) ^ c;
}

int readGame(int row0[3], int row1[5], int row2[7], int *player) {
    FILE *file;
    char fileName[31]; // String to store the user-entered file name
    char playerChar;
    printf(SPACER);
    printf("Reading Game from File\n");
    printf("Enter the name of the file you would like to load the game from (30 character limit): ");
    scanf("%30s", fileName);
    file = fopen(fileName, "r"); // Try opening the file the user gave

    if (file == NULL) { // If the file does not exist...
        printf("There is no file called %s. Returning to main menu...\n", fileName);
        printf(SPACER);
        return 0; // Return to the main loop as a failure
    }

    // Otherwise, if the file does exist, read it row by row
    readRow(file, row0);
    readRow(file, row1);
    readRow(file, row2);
    fscanf(file, "%c", &playerChar);
    *player = playerChar == 'B'; // Set the player boolean from the file

    fclose(file);
    return 1;
}

int rowSum(int row[], int size) {
    int sum = 0;
    int i;
    for (i = 0; i < size; i++)
        sum += row[i]; // Add to the sum if this index of the array is 1
    return sum;
}

int writeGame(int row0[3], int row1[5], int row2[7], int player) {
    FILE *file;
    char fileName[31]; // String to store the user-entered file name
    char createNewFile; // To get input from the user later
    printf(SPACER);
    printf("Saving Game to File\n");
    printf("Enter the name of the file you would like to save this game to (this will overwrite existing files) (30 character limit): ");
    scanf("%30s", fileName);
    file = fopen(fileName, "r"); // FIRST try to open the file in read mode to see if it exists

    if (file == NULL) { // If it does not exist, give the user the option to create it
        printf("There is no file called %s. Do you want to create it? (Y/n) ", fileName);
        scanf(" %c", &createNewFile);
        if (createNewFile != 'y' && createNewFile != 'Y') { // If the user DOES NOT enter yes, return to the game
            printf("Returning to game...\n");
            return 0; // Return 0 because the file was not saved
        }
    } else { // Otherwise, make sure to close the file before the next step
        fclose(file);
    }

    file = fopen(fileName, "w"); // NOW open it in write mode. If it did not exist before, it will be created

    // Write to the file row by row
    writeRow(file, row0, 3);
    writeRow(file, row1, 5);
    writeRow(file, row2, 7);
    fprintf(file, "%c.", player ? 'B' : 'A');

    fclose(file);
    printf("Game saved. Exiting...");
    return 1;
}

void firstAvailableMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pieces) {
    *pieces = 1; // Only take one piece
    // Take it from whichever row has a piece to be taken
    if (rowSum(row0, 3))
        *chosenRow = 1;
    else if (rowSum(row1, 5))
        *chosenRow = 2;
    else
        *chosenRow = 3;
}

void getAIMove(int row0[3], int row1[5], int row2[7], int *chosenRow, int *pieces) {
    int X = nimSum(row0, row1, row2);
    int h1, h2, h3;
    int h1SumX, h2SumX, h3SumX;

    if (X == 0) // This means we are not in a guaranteed winning position; make a dummy move to move the game along
        return firstAvailableMove(row0, row1, row2, chosenRow, pieces); // Call and exit

    // Calculate important values for checks
    h1 = rowSum(row0, 3);
    h1SumX = X ^ h1;

    h2 = rowSum(row1, 5);
    h2SumX = X ^ h2;

    h3 = rowSum(row2, 7);
    h3SumX = X ^ h3;

    if (h1 + h2 + h3 == 1 || h1 == 1 && h2 == 1 && h3 == 1) // Sorry buddy you lost (there is no way to win)
        return firstAvailableMove(row0, row1, row2, chosenRow, pieces);

    // Special cases 1
    if (h1 + h2 == 1 && h3 < 4 || // There is a one, a zero, and something else
        h1 + h3 == 1 && h2 < 4 || // There is a one, a zero, and something else
        h2 + h3 == 1 && h1 < 4) { // There is a one, a zero, and something else
        if (h1 > 1) {
            *chosenRow = 1;
            *pieces = h1;
        } else if (h2 > 1) {
            *chosenRow = 2;
            *pieces = h2;
        } else {
            *chosenRow = 3;
            *pieces = h3;
        }
        return;
    }

    // Special cases 2
    if (h1 == 1 && h2 == 1 && h3 < 4 || // There are two ones and something else
        h1 == 1 && h3 == 1 && h2 < 4 || // There are two ones and something else
        h2 == 1 && h3 == 1 && h1 < 4 || // There are two ones and something else
        h1 == 0 && h2 == 0 && h3 < 5 || // There are two zeros and something else
        h1 == 0 && h3 == 0 && h2 < 5 || // There are two zeros and something else
        h2 == 0 && h3 == 0 && h1 < 5) { // There are two zeros and something else
        // Make the proper move to get the game into a winning position
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

    // Standard strategy calculations now to get nimSum back to 0
    if (h1SumX <= h1 && h1 - h1SumX <= 3) {
        *chosenRow = 1;
        *pieces = h1 - h1SumX;
    } else if (h2SumX <= h2 && h2 - h2SumX <= 3) {
        *chosenRow = 2;
        *pieces = h2 - h2SumX;
    } else if (h3SumX <= h3 && h3 - h3SumX <= 3) {
        *chosenRow = 3;
        *pieces = h3 - h3SumX;
    } else { // Backup move if there is no way to make the right move (if the proper move would be more than 3 pieces)
        firstAvailableMove(row0, row1, row2, chosenRow, pieces);
    }
}

void displayBoard(int row0[3], int row1[5], int row2[7]) {
    printRow(row0, 1, 3);
    printRow(row1, 2, 5);
    printRow(row2, 3, 7);
}

void printRow(int *row, int number, int size) {
    int i;
    printf(ROW_LABEL"Row %d%*c"RESET, number, 10 - size, ' '); // The label on the left ("Row 1")
    for (i = 0; i < size; i++) {
        if (*(row + i)) // Dereference the correct index of the array
            printf(BOARD_BG" "FULL_PIECE"■"RESET); // Print with the full color
        else
            printf(BOARD_BG" "EMPTY_PIECE"■"RESET); // Print with the empty color
    }
    printf(BOARD_BG" "RESET"\n"); // After each row, make sure to move to the next line
}

void readRow(FILE *file, int row[]) {
    char c = ' ';
    int i = 0;
    while (c != '.') { // As long as we do not reach the end marker, which is a period
        fscanf(file, "%c", &c);
        if (c == ',') // If this character is a comma, move on without doing anything because this separates items
            continue;
        else if (c == 'E') // If E, set as 0 for empty
            row[i] = 0;
        else if (c == 'F') // If F, set as 1 for full
            row[i] = 1;
        i++; // This will not increment when the character is a comma
    }

    // Move the cursor over one character to skip the newline and prepare for the next call of this function
    fseek(file, sizeof(char), SEEK_CUR);
}

void removePieces(int row[], int pieces) {
    int removed = 0; // Track how many pieces have actually been removed
    int i = 0;
    while (removed < pieces) { // Go until the right number of pieces has been removed
        removed += row[i]; // Removed will only increase if there was actually something there to remove
        row[i] = 0;
        i++; // Move over one place in the array each time regardless
    }
}

void setUpAI(int *aiPlayer) {
    int input;

    printf(SPACER);

    while (1) { // Loop until broken
        printf("Who goes first?\n1: I go first\n2: Computer goes first\n3: Randomize\nEnter selection: ");
        scanf("%d", &input);
        if (input == 1) { // If 1, the human player goes first
            printf("Ok. You are player "PLAYER"A"RESET".  Preparing new game...\n");
            *aiPlayer = 1; // Set the computer to be player B
        } else if (input == 2) { // If 2, the computer goes first
            printf("Ok. You are player "PLAYER"B"RESET".  Preparing new game...\n");
            *aiPlayer = 0; // Set the computer to be player A
        } else if (input == 3) { // If 3, pick a random number to determine who goes first
            *aiPlayer = rand() % 2; // Modulo 2 to get a number between 0 and 1
            printf("Ok. You are player "PLAYER"%c"RESET".  Preparing new game...\n", *aiPlayer ? 'A' : 'B');
        } else { // If the user gave an invalid option, go back to the start of the loop using continue
            printf("Invalid option. Type a number between 1 and 3\n");
            continue;
        }
        break; // If the program gets here, an option was chosen successfully, and the loop can break
    }
}

void setUpGame(int row0[3], int row1[5], int row2[7], int *player, int *computerGame, int *aiPlayer) {
    int input;

    printf(SPACER);

    while (1) { // Loop until broken
        printf("What would you like to do?\n1: Start new game\n2: Load game from file\n3: Start new game against computer\nEnter selection: ");
        scanf("%d", &input);
        if (input == 1) { // If 1, nothing to do here except go back to the main loop and get started
            printf("Ok. Preparing new game...\n");
        } else if (input == 2) { // If 2, attempt to read a game from a file
            if (!readGame(row0, row1, row2, player)) // If the read fails and returns 0...
                continue; // Go back to the start of this loop and prompt the user for an option again with continue
        } else if (input == 3) { // If 3, set the computer game flag and prompt the user for computer options
            printf("Ok. Preparing new game against computer...\n");
            *computerGame = 1;
            setUpAI(aiPlayer);
        } else { // If the user gave an invalid option, go back to the start of the loop using continue
            printf("Invalid option. Type a number between 1 and 3\n");
            continue;
        }
        break; // If the program gets here, an option was chosen successfully, and the loop can break
    }
}

void writeRow(FILE *file, int row[], int size) {
    int i;
    for (i = 0; i < size; i++) { // For each item in the array
        fprintf(file, "%c", row[i] ? 'F' : 'E'); // First, print an F or E depending on whether a piece is there
        fprintf(file, "%c", i == size - 1 ? '.' : ','); // Then, print a comma, or, if this is the last item, a period
    }
    fprintf(file, "\n"); // Print a newline at the end to prepare for the next line to be written
}

/**
* External Sources
* [Stack Overflow](https://stackoverflow.com/questions/8464620/program-doesnt-wait-for-user-input-with-scanfc-yn) on bug with scanf after using scanf to get string input from user
* [Stack Overflow](https://stackoverflow.com/questions/32674141/if-file-pointer-is-null-do-i-have-to-use-fclose-c) on bug only in Repl.it of files not being created
* [cppreference.com](https://en.cppreference.com/w/c/io/fseek) on specifics of fseek
* [Wikipedia](https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit) on ANSI escape codes for colored text
* [Wikipedia](https://en.wikipedia.org/wiki/Nim#Proof_of_the_winning_formula) Nim mathematical strategy
*/