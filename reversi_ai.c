//
// Author: Kaitlyn Chit
//

#include "reversi_ai.h"

#include <stdbool.h>
#include <stdio.h>

void initializeBoard(char board[][26], int n) {
  // set all positions to 'U'
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      board[i][j] = 'U';
    }
  }

  // set starting 2x2 tiles
  int mid = n / 2;
  board[mid - 1][mid - 1] = 'W';
  board[mid - 1][mid] = 'B';
  board[mid][mid - 1] = 'B';
  board[mid][mid] = 'W';
}

void printBoard(char board[][26], int n) {
  // print column labels
  printf("  ");
  for (int j = 0; j < n; j++) {
    printf("%c", 'a' + j);
  }
  printf("\n");

  // print each row
  for (int i = 0; i < n; i++) {
    printf("%c ", 'a' + i);
    for (int j = 0; j < n; j++) {
      printf("%c", board[i][j]);
    }
    printf("\n");
  }
}

bool checkLegalInDirection(char board[][26], int n, int row, int col,
                           char colour, int deltaRow, int deltaCol) {
  // determine opponent's color
  char opponent;
  if (colour == 'W') {
    opponent = 'B';
  } else {
    opponent = 'W';
  }

  int r = row + deltaRow;  // start one step away from the chosen square
                           // (currently empty)
  int c = col + deltaCol;

  bool foundOpponent = false;  // have we seen at least one opponent tile?

  // Step along the line
  while (r >= 0 && r < n && c >= 0 && c < n) {  // make sure we are in bounds
    if (board[r][c] == opponent) {
      foundOpponent = true;  // found at least one opponent tile
    } else if (board[r][c] == colour) {
      // we found our own tile
      return foundOpponent;
    } else {
      // empty space
      return false;
    }
    r += deltaRow;  // move to next square in this direction
    c += deltaCol;
  }

  // reached edge of board without finding our tile
  return false;
}

bool isLegalMove(char board[][26], int n, int row, int col, char colour) {
  if (board[row][col] != 'U') {
    return false;  // must be empty
  }
  for (int r = -1; r <= 1; r++) {
    for (int c = -1; c <= 1; c++) {
      if (r == 0 && c == 0) {
        continue;
      }
      if (checkLegalInDirection(board, n, row, col, colour, r, c)) {
        return true;
      }
    }
  }
  return false;
}

void placeTileAndFlip(char board[][26], int n, int row, int col, char colour) {
  board[row][col] = colour;
  for (int dr = -1; dr <= 1; dr++) {
    for (int dc = -1; dc <= 1; dc++) {
      if (dr == 0 && dc == 0) {
        continue;
      }
      if (checkLegalInDirection(board, n, row, col, colour, dr, dc)) {
        int r = row + dr;
        int c = col + dc;
        while (board[r][c] != colour) {
          board[r][c] = colour;
          r += dr;
          c += dc;
        }
      }
    }
  }
}

bool moveAvailable(char board[][26], int n, char colour) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (isLegalMove(board, n, i, j, colour)) {
        return true;
      }
    }
  }
  return false;
}

void printWinner(char board[][26], int n) {
  int blackCount = 0;
  int whiteCount = 0;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (board[i][j] == 'B') {
        blackCount++;
      }
      if (board[i][j] == 'W') {
        whiteCount++;
      }
    }
  }

  if (blackCount > whiteCount) {
    printf("B player wins.\n");
  } else if (whiteCount > blackCount) {
    printf("W player wins.\n");
  } else {
    printf("Draw!\n");
  }
}

// count how many moves are available for a colour on a given board
int countAvailableMoves(char board[][26], int n, char colour) {
  int count = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (isLegalMove(board, n, i, j, colour)) {
        count++;
      }
    }
  }
  return count;
}

// check if a position is one of the four corners
bool isCorner(int row, int col, int n) {
  if (row == 0 && col == 0) {
    return true;
  }
  if (row == 0 && col == n - 1) {
    return true;
  }
  if (row == n - 1 && col == 0) {
    return true;
  }
  if (row == n - 1 && col == n - 1) {
    return true;
  }
  return false;
}

// check if a position is adjacent to a corner (dangerous square)
bool isNextToCorner(int row, int col, int n) {
  // adjacent to top-left corner
  if (row <= 1 && col <= 1 && !isCorner(row, col, n)) {
    return true;
  }
  // adjacent to top-right corner
  if (row <= 1 && col >= n - 2 && !isCorner(row, col, n)) {
    return true;
  }
  // adjacent to bottom-left corner
  if (row >= n - 2 && col <= 1 && !isCorner(row, col, n)) {
    return true;
  }
  // adjacent to bottom-right corner
  if (row >= n - 2 && col >= n - 2 && !isCorner(row, col, n)) {
    return true;
  }
  return false;
}

// makeMove — required name/signature per reversi_ai.h's fixed interface.
// Scores each legal move using a weighted combination of capture count,
// corner control, danger-square avoidance, edge preference, and opponent
// mobility reduction.
int makeMove(const char board[][26], int n, char turn, int *row, int *col) {
  char opponent;
  if (turn == 'B') {
    opponent = 'W';
  } else {
    opponent = 'B';
  }

  int bestRow = -1;
  int bestCol = -1;
  int bestScore = -99999;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (!isLegalMove(board, n, i, j, turn)) {
        continue;
      }

      // copy the board
      char copy[26][26];
      for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
          copy[r][c] = board[r][c];
        }
      }

      // count opponent tiles before
      int before = 0;
      for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
          if (copy[r][c] == opponent) {
            before++;
          }
        }
      }

      placeTileAndFlip(copy, n, i, j, turn);

      // count opponent tiles after
      int after = 0;
      for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
          if (copy[r][c] == opponent) {
            after++;
          }
        }
      }

      int flipsScore = before - after;

      // corner bonus - corners are very valuable
      int cornerScore = 0;
      if (isCorner(i, j, n)) {
        cornerScore = 100;
      }

      // penalty for squares next to corners - giving opponent corner access is
      // bad
      int dangerScore = 0;
      if (isNextToCorner(i, j, n)) {
        dangerScore = -50;
      }

      // small bonus for edge squares that are not corners or next to corners
      int edgeScore = 0;
      if (!isCorner(i, j, n) && !isNextToCorner(i, j, n)) {
        if (i == 0 || i == n - 1 || j == 0 || j == n - 1) {
          edgeScore = 10;
        }
      }

      // count how many moves opponent has after our move - weight this heavily
      int opponentMoves = countAvailableMoves(copy, n, opponent);
      int opponentMovesScore = -3 * opponentMoves;
 
      int totalScore = flipsScore + cornerScore + dangerScore + edgeScore + opponentMovesScore;
 
      if (totalScore > bestScore) {
        bestScore = totalScore;
        bestRow = i;
        bestCol = j;
      }
    }
  }
 
  *row = bestRow;
  *col = bestCol;
  return 0;
}

int main(void) {
  int n;
  char board[26][26];

  printf("Enter the board dimension: ");
  scanf("%d", &n);

  char colourInput[3];
  printf("Computer plays (B/W): ");
  scanf("%s", colourInput);
  char computerColour = colourInput[0];

  initializeBoard(board, n);
  printBoard(board, n);

  char turn = 'B';

  while (true) {
    if (!moveAvailable(board, n, 'B') && !moveAvailable(board, n, 'W')) {
      break;
    }

    if (!moveAvailable(board, n, turn)) {
      printf("%c player has no valid move.\n", turn);
      if (turn == 'B') {
        turn = 'W';
      } else {
        turn = 'B';
      }
      continue;
    }

    if (turn == computerColour) {
      // heuristic bot makes a move
      int row, col;
      makeMove(board, n, computerColour, &row, &col);
      placeTileAndFlip(board, n, row, col, computerColour);
      printf("Computer places %c at %c%c.\n", computerColour, 'a' + row,
             'a' + col);
      printBoard(board, n);
    } else {
      // human plays the other colour
      char move[4];
      printf("Enter move for colour %c (RowCol): ", turn);
      scanf("%s", move);

      int row = move[0] - 'a';
      int col = move[1] - 'a';

      if (!isLegalMove(board, n, row, col, turn)) {
        printf("Invalid move.\n");
        if (turn == 'B') {
          printf("W player wins.\n");
        } else {
          printf("B player wins.\n");
        }
        return 0;
      }

      placeTileAndFlip(board, n, row, col, turn);
      printBoard(board, n);
    }

    if (turn == 'B') {
      turn = 'W';
    } else {
      turn = 'B';
    }
  }

  printWinner(board, n);
  return 0;
}