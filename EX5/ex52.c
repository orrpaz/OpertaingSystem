// Or Paz
// 311250708
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define SIZE 20

#define EMPTY ' '
#define STAR '*'
#define FORM '-'

/**
 * status of the shape.
 */
typedef enum {
    LAY, STAND
} status;

/**
 * enum if we need change the shape.
 */
typedef enum {
    DONT_CHANGE, CHANGE
} Flip;

/**]
 * struct.
 */
typedef struct GameBoard {
    char board[SIZE][SIZE];
    int row;
    int col;
    status mode;
} GameBoard;

// global variable.
GameBoard gameBoard;
/**
 *
 * @param x
 * @param y
 * @param flip
 */
void updatePosition( int x, int y, Flip flip);
/**
 * this method init the game.
 */
void initialize();
/**
 * this method clear the shape from board.
 */
void clearShape();
/**
 * this method clear the board. put STARS AND BACKSPACE in array.
 */
void clearBoard();
/**
 * this method print the board.
 */
void printBoard();
/**
 * this method change the mode of shape
 */
void changeMode();
/**
 * this method is signal method. check which key is entered
 * @param sig
 */
void SignalGetKeyFromUser(int sig);
/**
 * this method is signal method do alarm and update new position.
 * @param sig
 */
void SignalMove(int sig);
/**
 * this method check if the next step is legal move.
 * @param y - column
 * @return 1 if it is legal move. 0- otherwise
 */
int isLegalMove(int y);
/**
 * this method check if change shpae is legal according to border.
 * @return
 */
int isLegalChange();


void printBoard() {
    system("clear");
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            printf("%c", gameBoard.board[i][j]);
        }
        printf("\n");
    }

}


void clearShape() {
    if (gameBoard.mode == STAND) {
        gameBoard.board[gameBoard.row][gameBoard.col] = EMPTY;
        gameBoard.board[gameBoard.row + 1][gameBoard.col] = EMPTY;
        gameBoard.board[gameBoard.row + 2][gameBoard.col] = EMPTY;
    } else {
        gameBoard.board[gameBoard.row][gameBoard.col] = EMPTY;
        gameBoard.board[gameBoard.row][gameBoard.col + 1] = EMPTY;
        gameBoard.board[gameBoard.row][gameBoard.col + 2] = EMPTY;
    }
}

void initialize() {
    gameBoard.mode = LAY;
    gameBoard.row = 0;
    gameBoard.col = (SIZE / 2) - 1;
    clearBoard();
    updatePosition( gameBoard.row, gameBoard.col, DONT_CHANGE);

}
int isLegalMove( int y){
    if (gameBoard.mode == STAND ) {
        if (y <= 0 || y >= SIZE - 1)
            return 0;

    }
    // "---" check bound
    if (gameBoard.mode == LAY) {
        if (y <= 0 || y  >= SIZE -3)
            return 0;
    }
    return 1;
}
void initShape(GameBoard* gameBoard, int x, int y) {
    if (gameBoard->mode == LAY) {
        gameBoard->board[x][y] = FORM;
        gameBoard->board[x][y + 1] = FORM;
        gameBoard->board[x][y + 2] = FORM;
    } else {
        gameBoard->board[x][y] = FORM;
        gameBoard->board[x + 1][y] = FORM;
        gameBoard->board[x + 2][y] = FORM;
    }
}
void isGoDown(int x){
    if(gameBoard.mode ==LAY) {
        if (x >= SIZE - 1) {
            initialize();
        }
    }else{
        if (x >= SIZE - 3) {
            initialize();
        }
    }
}
void updatePosition( int x, int y, Flip flip) {
    if(isLegalMove(y)) {

        // erase previous shape
        if (flip == DONT_CHANGE)
            clearShape();

        gameBoard.row = x;
        gameBoard.col = y;
        initShape(&gameBoard,x,y);
        // check if we arrive to the bottom.
        isGoDown(x);
    }
}



void clearBoard() {
    int i, j;
    for (i = 0; i < SIZE - 1; i++) {
        for (j = 0; j < SIZE; j++) {
            if (j == 0 || j == SIZE - 1) {
                gameBoard.board[i][j] = STAR;
                continue;
            }
            gameBoard.board[i][j] = EMPTY;
        }
    }
    // put '*' on bottom
    for (j = 0; j < SIZE; j++) {
        gameBoard.board[SIZE - 1][j] = STAR;
    }
}

void SignalGetKeyFromUser(int sig) {

    char letter;
    // we make dup2 therefore scanf is possible.
    scanf("%c",&letter);
    switch (letter) {
        case 'a':
            // LEFT
            updatePosition( gameBoard.row, gameBoard.col - 1, DONT_CHANGE);
            break;
        case 's':
            // DOWN
            updatePosition( gameBoard.row + 1, gameBoard.col, DONT_CHANGE);
            break;
        case 'd':
            // RIGHT
            updatePosition( gameBoard.row, gameBoard.col + 1, DONT_CHANGE);
            break;
        case 'w':
            // CHANGE FORM
           if(!isLegalChange()){
               break;
           }
            clearShape();
            changeMode();
            break;
        case 'q': exit(1);
        default:
            break;
    }
    printBoard();
    signal(SIGUSR2, SignalGetKeyFromUser);

}

int isLegalChange(){
    if(gameBoard.mode == STAND){
    	// the shape in the side of board.
        if(gameBoard.col == SIZE - 2  || gameBoard.col == 1){
           return 0;
        }
    }
    return 1;
}

void changeMode() {
    gameBoard.mode = (gameBoard.mode == LAY) ? STAND : LAY;

    if (gameBoard.mode == STAND) {
        updatePosition(gameBoard.row -1, gameBoard.col + 1, CHANGE);
    } else  {
        // HORIZONTAL
        // left bound
        if(gameBoard.col <= 1)
            gameBoard.col++;
        // right bound
        if (gameBoard.col >= SIZE - 2) {
            gameBoard.col--;
        }


        updatePosition( gameBoard.row + 1, gameBoard.col - 1, CHANGE);
    }
}
void SignalMove(int sig) {
    alarm(1);
    updatePosition( gameBoard.row + 1, gameBoard.col, DONT_CHANGE);
    printBoard();
    signal(SIGALRM, SignalMove);

}


int main() {

    initialize();
    printBoard();
    signal(SIGALRM, SignalMove);
    alarm(1);


    //free(gameBoard);
    signal(SIGUSR2, SignalGetKeyFromUser);
    while (1) {
        pause();
    }
}


