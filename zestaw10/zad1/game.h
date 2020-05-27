#define MAX_PLAYERS 20
#define MAX_BACKLOG 10
#define MAX_MESSAGE_LENGTH 256

typedef enum { E, O, X } board_object;

typedef struct {
    int current_move;
    board_object objects[9];
} board_t;

board_t new_board() {
    board_t board = {1, {E}};
    return board;
}

int make_move(board_t *board, int position){
    if (position > 9 || position < 0){
        return 0;
    }
        
    if (board -> objects[position] != E){
        return 0;
    }
    if (board -> current_move == O){
        board -> objects[position] = O;
        board -> current_move = X;
    } else {
        board -> objects[position] = X;
        board -> current_move = O;
    }
    return 1;
}

board_object column_win(board_t *board) {
    for (int x = 0; x < 3; x++) {
        board_object b1 = board -> objects[x];
        board_object b2 = board -> objects[x + 3];
        board_object b3 = board -> objects[x + 6];
        if (b1 == b2 && b1 == b3 && b1 != E){
            return b1;
        } 
    }
    return E;
}

board_object row_win(board_t *board) {
    for (int y = 0; y < 3; y++) {
        board_object b1 = board -> objects[3 * y];
        board_object b2 = board -> objects[3 * y + 1];
        board_object b3 = board -> objects[3 * y + 2];
        if (b1 == b2 && b1 == b3 && b1 != E) {
            return b1;
        }
    }
    return E;
}

board_object diagonal_win(board_t *board) {
    board_object b1 = board -> objects[0];
    board_object b2 = board -> objects[4];
    board_object b3 = board -> objects[8];
    if (b1 == b2 && b1 == b3 && b1 != E){
        return b1;
    } 

    board_object b4 = board->objects[2];
    board_object b5 = board->objects[4];
    board_object b6 = board->objects[7];
    if (b4 == b5 && b4 == b6 && b4 != E){
        return b4;
    } 

    return E;
}

board_object get_winner(board_t *board) {
    board_object column = column_win(board);
    board_object row = row_win(board);
    board_object diagonal = diagonal_win(board);

    if (column != E){
        return column;
    }

    if (row != E){
        return row;
    }

    return diagonal;
}
