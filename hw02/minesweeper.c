#include "minesweeper.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UNUSED(A) (void)(A)
#define UNKNOWN (uint16_t) 9
#define MINE (uint16_t) 10
#define HIDDEN (uint16_t) 11
#define FLAG (uint16_t) 50

uint16_t check_around(size_t x, size_t y, size_t rows, size_t cols,
                      uint16_t board[rows][cols]);
void print_border(size_t cols);
void print_row(size_t rows, size_t cols, uint16_t board[rows][cols],
               int row_number);
void fill(size_t rows, size_t cols, uint16_t board[rows][cols]);

/* ************************************************************** *
 *                         HELP FUNCTIONS                         *
 * ************************************************************** */

bool is_flag(uint16_t cell) { return cell >= FLAG && cell <= 71; }

bool is_mine(uint16_t cell) {
  return (cell == MINE || cell == MINE + HIDDEN ||
          cell == MINE + HIDDEN + FLAG);
}

bool is_revealed(uint16_t cell) { return cell < HIDDEN; }

int get_number(uint16_t cell) {
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

bool is_number(uint16_t cell) {
  return cell <= 8 || (cell >= HIDDEN && cell <= HIDDEN + 8);
}

/* ************************************************************** *
 *                         INPUT FUNCTIONS                        *
 * ************************************************************** */

bool set_cell(uint16_t *cell, char val) {
  if (cell == NULL) {
    return false;
  }
  switch (toupper(val)) {
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

  if (val >= '0' && val <= '8') {
    *cell = val - '0';
    return true;
  }

  return false;
}

int load_board(size_t rows, size_t cols, uint16_t board[rows][cols]) {
  size_t count = 0;
  bool eof = false;

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      uint16_t cell = 0;
      int val = getchar();

      if (isspace(val)) {
        j--;
        continue;
      }
      if (val == EOF) {
        eof = true;
        break;
      }

      if (set_cell(&cell, val)) {
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

int postprocess(size_t rows, size_t cols, uint16_t board[rows][cols]) {
  if (rows < MIN_SIZE || rows > MAX_SIZE || cols < MIN_SIZE || cols > MAX_SIZE) {
    return -1;
  }

  size_t mines_amount = 0;

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {

      uint16_t *cell = &board[i][j];

      if (is_mine(*cell)) {
        mines_amount++;

        if ((i == 0 && j == 0) || (i == 0 && j == cols - 1) ||
            (i == rows - 1 && j == 0) || (i == rows - 1 && j == cols - 1)) {
          return -1;
        }
        continue;
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
        continue;
      }
      if (*cell == UNKNOWN + HIDDEN + FLAG) {
        *cell = (check_around(i, j, rows, cols, board)) + HIDDEN + FLAG;
        continue;
      }

      if (*cell == UNKNOWN + HIDDEN) {
        *cell = (check_around(i, j, rows, cols, board)) + HIDDEN;
        continue;
      }

      if (*cell == UNKNOWN) {
        *cell = (check_around(i, j, rows, cols, board));
        continue;
      }

      return -1;
    }
  }

  if (mines_amount <= 0) {
    return -1;
  }

  return mines_amount;
}

uint16_t check_around(size_t x, size_t y, size_t rows, size_t cols,
                      uint16_t board[rows][cols]) {
  uint16_t mine_count = 0;

  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {

      if ((x == 0 && i == -1) || (y == 0 && j == -1)) {
        continue;
      }

      if ((x == rows - 1 && i == 1) || (y == cols - 1 && j == 1)) {
        continue;
      }

      if (is_mine(board[x + i][y + j])) {
        mine_count++;
      }
    }
  }

  return mine_count;
}

/* ************************************************************** *
 *                        OUTPUT FUNCTIONS                        *
 * ************************************************************** */

int print_board(size_t rows, size_t cols, uint16_t board[rows][cols]) {
  printf("   ");
  for (size_t i = 0; i <= cols - 1; i++) {
    if (i < 10) {
      printf("  %lu ", i);
    } else {
      printf(" %lu ", i);
    }
  }

  putchar('\n');

  for (size_t i = 0; i <= rows * 2; i++) {

    if (i % 2 == 1) {

      if (i / 2 < 10) {
        printf(" %lu ", i / 2);
      } else {
        printf("%lu ", i / 2);
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

  if (is_flag(cell)) {
    return 'F';
  }

  if (cell >= 11 && cell <= 21) {
    return 'X';
  }

  if (cell >= 1 && cell <= 8) {
    return cell + '0';
  }

  return ' ';
}

void print_border(size_t cols) {
  for (size_t i = 0; i < cols; i++) {

    printf("+---");
  }

  printf("+\n");
}

void print_row(size_t rows, size_t cols, uint16_t board[rows][cols],
               int row_number) {
  for (size_t i = 0; i < cols; i++) {

    char val = show_cell(board[row_number][i]);

    switch (val) {
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
      printf("| %c ", val);
      break;
    }
  }

  printf("|\n");
}

/* ************************************************************** *
 *                    GAME MECHANIC FUNCTIONS                     *
 * ************************************************************** */

int reveal_cell(size_t rows, size_t cols, uint16_t board[rows][cols],
                size_t row, size_t col) {
  if (row > rows - 1 || col > cols - 1) {
    return -1;
  }

  if (is_revealed(board[row][col]) || is_flag(board[row][col])) {
    return -1;
  }

  if (board[row][col] == UNKNOWN || board[row][col] == UNKNOWN + HIDDEN) {
    return -1;
  }

  if ((board[row][col] > 21 && board[row][col] < 50) || board[row][col] > 71) {  //if it's a undefined value
    return -1; 
  }

  int result = reveal_single(&board[row][col]);

  if (result == 0 && board[row][col] == 0) {
    reveal_floodfill(rows, cols, board, row, col);
  }

  return result;
}

int reveal_single(uint16_t *cell) {
  if (cell == NULL) {
    return -1;
  }

  if (is_revealed(*cell) || is_flag(*cell)) {
    return -1;
  }

  if (*cell == UNKNOWN || *cell == UNKNOWN + HIDDEN) {
    return -1;
  }

  if ((*cell > 21 && *cell < 50) || *cell > 71) {  //if it's a undefined value
    return -1; 
  }

  *cell -= HIDDEN;

  if (is_mine(*cell)) {
    return 1;
  }

  return 0;
}

void reveal_floodfill(size_t rows, size_t cols, uint16_t board[rows][cols],
                      size_t row, size_t col) {
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {

      if ((row == 0 && i == -1) || (col == 0 && j == -1)) {
        continue;
      }

      if ((row == rows - 1 && i == 1) || (col == cols - 1 && j == 1)) {
        continue;
      }

      if (!is_revealed(board[row + i][col + j])) {
        if (board[row + i][col + j] >= 61 &&
            board[row + i][col + j] <= 69) { // if it's a wrong flag
          board[row + i][col + j] -= FLAG;   // remove flag
        }
        reveal_cell(rows, cols, board, row + i, col + j);
      }
    }
  }
}

int flag_cell(size_t rows, size_t cols, uint16_t board[rows][cols], size_t row,
              size_t col) {

  if (row < rows && col < cols) {

    if (is_flag(board[row][col])) {

      board[row][col] -= FLAG;
    } else {

      board[row][col] += FLAG;
    }
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

bool is_solved(size_t rows, size_t cols, uint16_t board[rows][cols]) {
  int count = 0;

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {

      uint16_t cell = board[i][j];

      if (!is_revealed(cell)) {
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

int generate_random_board(size_t rows, size_t cols, uint16_t board[rows][cols],
                          size_t mines) {
  
  if (rows < MIN_SIZE || cols < MIN_SIZE || rows > MAX_SIZE || cols > MAX_SIZE) {
    return -1;
  }

  fill(rows, cols, board);
  
  while (mines > 0) {

    size_t row = rand() % rows;
    size_t col = rand() % rows;

    if ((row == 0 && col == 0) || (row == 0 && col == cols - 1) ||
        (row == rows - 1 && col == 0) || (row == rows - 1 && col == cols - 1)) {
      continue;
    }

    if (!is_mine(board[row][col])) {
      board[row][col] = MINE + HIDDEN;
      mines--;
    }
  }

  return postprocess(rows, cols, board);
}

void fill(size_t rows, size_t cols, uint16_t board[rows][cols])
{
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      board[i][j] = UNKNOWN + HIDDEN;
    }
  }
}

int find_mines(size_t rows, size_t cols, uint16_t board[rows][cols]) {
  // TODO: Implement me
  UNUSED(rows);
  UNUSED(cols);
  UNUSED(board);
  return -1;
}
