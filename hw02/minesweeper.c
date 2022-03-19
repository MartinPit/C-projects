#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include "minesweeper.h"

#define UNUSED(A) (void) (A)
#define UNKNOWN ((uint16_t) 9)
#define MINE ((uint16_t) 10)
#define HIDDEN ((uint16_t) 11)
#define FLAG ((uint16_t) 50)

uint16_t check_around(size_t x, size_t y, size_t rows, size_t cols, uint16_t board[rows][cols]);
void print_border(size_t cols);
void print_row(size_t rows, size_t cols, uint16_t board[rows][cols], int row_number);




/* ************************************************************** *
 *                         HELP FUNCTIONS                         *
 * ************************************************************** */

bool is_flag(uint16_t cell)
{
    return cell >= FLAG;
}

bool is_mine(uint16_t cell)
{
    return (cell == MINE || cell == MINE + HIDDEN || cell == MINE + HIDDEN + FLAG);

}

bool is_revealed(uint16_t cell)
{
    return cell < HIDDEN;
}

int get_number(uint16_t cell)
{
    if (is_mine(cell)) {
        return 0;
    }

    if (cell >= 11 && cell <= 19) {
        return cell - HIDDEN;
    }

    if (cell >= 61 && cell <= 69) {
        return cell - HIDDEN - FLAG;
    }

    return cell;
}

bool is_number(uint16_t cell)
{
    return cell <= 8 || (cell >= HIDDEN && cell <= HIDDEN + 8);
}

/* ************************************************************** *
 *                         INPUT FUNCTIONS                        *
 * ************************************************************** */

bool set_cell(uint16_t *cell, char val)
{   
    if (cell == NULL) {
        return false;
    }
    switch (toupper(val))
    {
    case 'M':
        *cell = MINE + HIDDEN;
        return true;
    
    case 'X':
        *cell = UNKNOWN + HIDDEN;
        return true;
    
    case 'F':
        *cell = MINE + HIDDEN + FLAG;
        return true;
    
    case 'W':
        *cell = UNKNOWN + HIDDEN + FLAG;
        return true;
    
    case '.':
        *cell = UNKNOWN;
        return true;
    }

    for(uint16_t i = 0; i < 9; i++) {
        if(val - 48 == i) {
            *cell = i;
            return true;
        }
    }
    return false;
}

int load_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    size_t count = 0;
    bool eof = false;

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {

            uint16_t cell;
            int val = getchar();

            if (val == EOF) {
                eof = true;
                break;
            }

            if(set_cell(&cell, val)) {
                count++;
                board[i][j] = cell;
            } else {
                j--;
            }
        }
        if (eof) {
            break;
        }
    }

    if (count != rows * cols) {
        return -1;
    }

    return postprocess(rows, cols, board);
}

int postprocess(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    if (rows < 3 || rows > 99 || cols < 3 || cols > 99) {
        return -1;
    }

    size_t mines_amount = 0;

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < rows; j++) {

            uint16_t *cell = &board[i][j];

            if (is_mine(*cell)) {
                mines_amount++;
                
                if ( (i == 0 && j == 0) || ( i == 0 && j == cols - 1) ||
                     (i == rows - 1 && j == 0) || (i == rows -1 && j == cols - 1)) {
                         return - 1;
                 }
            }

            if (is_number(*cell)) {
                if (*cell >= HIDDEN) {
                    if (check_around(i, j, rows, cols, board) != *cell - HIDDEN) {
                        return -1;
                    }
                }
                if (check_around(i, j, rows, cols, board) != *cell) {
                    return -1;
                }
            }
            if (*cell == UNKNOWN + HIDDEN + FLAG) {
                *cell = (check_around(i, j, rows, cols, board)) + HIDDEN + FLAG;
            }

            if (*cell == UNKNOWN + HIDDEN) {
                *cell = (check_around(i, j, rows, cols, board)) + HIDDEN;
            }

            if (*cell == UNKNOWN) {
                *cell = (check_around(i, j, rows, cols, board));
            }
        }
    }

    if (mines_amount <= 0) {
        return -1;
    }

    return mines_amount;
}

uint16_t check_around(size_t x, size_t y, size_t rows, size_t cols, uint16_t board[rows][cols])
{
    uint16_t mine_count = 0;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {

            int x_check = ((int) x) + i;
            int y_check = ((int) y) + j;

            if (x_check < 0 || y_check < 0 || x_check >= (int) cols || y_check >= (int) rows) {
                continue;
            }

            if (is_mine(board[x_check][y_check])) {
                mine_count++;
            }
        }
    }

    return mine_count;
}

/* ************************************************************** *
 *                        OUTPUT FUNCTIONS                        *
 * ************************************************************** */

int print_board(size_t rows, size_t cols, uint16_t board[rows][cols])
{   
    printf("   ");
    for (int i = 0; i <= (int) cols - 1; i++) {
        if (i < 10) {
            printf("  %d ", i);
        } else {
            printf(" %d ", i);
        }
    }

    putchar('\n');

    for (int i = -1; i <= (int) rows * 2 - 1; i++) {

        if (i % 2 == 0) {

            if (i / 2 < 10) {
                printf(" %d ", i / 2);
            } else {
                printf("%d ", i / 2);
            }

            print_row(rows, cols, board, i / 2);
        
        } else {

            printf("   ");
            print_border(cols);
        }
    }

    return 0;
}

char show_cell(uint16_t cell)
{
    if (cell == MINE) {
        return 'M'; 
    }

    if (cell >= FLAG) {
        return 'F';
    }

    if (cell >= 11 && cell <= 21) {
        return 'X';
    }

    if (cell >= 1 && cell <= 8) {
        return (char) cell;
    }

    return ' ';
}

void print_border(size_t cols)
{
    for (int i = 0; i < (int) cols; i++) {

        printf("+---");
    }

    printf("+\n");
}

void print_row(size_t rows, size_t cols, uint16_t board[rows][cols], int row_number)
{
    for (int i = 0; i < (int) cols; i++) {

        char val = show_cell(board[row_number][i]);

        switch (val)
        {
        case 'M':
            printf("| M ");
            break;
        
        case 'X':
            printf("|XXX");
            break;
        
        case 'F':
            printf("|_F_");
            break;
        
        case ' ':
            printf("|   ");
            break;
        
        default:
            printf("| %d ", val);
            break;
        }
    }

    printf("|\n");
}

/* ************************************************************** *
 *                    GAME MECHANIC FUNCTIONS                     *
 * ************************************************************** */

int reveal_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (row > rows - 1 || col > cols - 1) {
        return -1;
    }

    int result = reveal_single(&board[row][col]);

    if (result == 0 && board[row][col] == 0) {
        reveal_floodfill(rows, cols, board, row, col);
    }

    return result;


}

int reveal_single(uint16_t *cell)
{
    if (is_revealed(*cell) || is_flag(*cell)) {
        return -1;
    }

    *cell -= HIDDEN;

    if (is_mine(*cell)) {
        return 1;
    }

    return 0;
}

void reveal_floodfill(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {

            int row_check = ((int) row) + i;
            int col_check = ((int) col) + j;

            if (row_check < 0 || col_check < 0 || row_check >= (int) cols || col_check >= (int) rows) {
                continue;
            }

            if(!is_revealed(board[row_check][col_check])) {
                if (board[row_check][col_check] >= 61 && board[row_check][col_check <= 69]) { // if it's a wrong flag
                    board[row_check][col_check] -= 50; // remove flag
                }
                reveal_cell(rows, cols, board, row_check, col_check);
            }

        }
    }
}

int flag_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row, size_t col)
{
    if (is_flag(board[row][col])) {
        board[row][col] -= FLAG;
    } else {
        board[row][col] += FLAG;
    }

    int mine_count = 0;

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            if (is_mine(board[i][j])) {
                mine_count++;

                if (is_flag(board[i][j])) {
                    mine_count--;
                }
            }
        }
    }

    return mine_count;
}

bool is_solved(size_t rows, size_t cols, uint16_t board[rows][cols])
{   
    int count = 0; 

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {

            uint16_t cell = board[i][j];

            if (! is_revealed(cell)) {
                count++;
            }

            if (is_mine(cell)) {
                count--;
            }
        }
    }
    return count == 0;
}

/* ************************************************************** *
 *                         BONUS FUNCTIONS                        *
 * ************************************************************** */

int generate_random_board(size_t rows, size_t cols, uint16_t board[rows][cols], size_t mines)
{
    // TODO: Implement me
    UNUSED(mines);

    // The postprocess function should be called at the end of the generate random board function
    return postprocess(rows, cols, board);
}

int find_mines(size_t rows, size_t cols, uint16_t board[rows][cols])
{
    // TODO: Implement me
    UNUSED(rows);
    UNUSED(cols);
    UNUSED(board);
    return -1;
}
