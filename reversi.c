//
// Author: Kaitlyn Chit
//

#include "reversi.h"

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

// renamed from computerMove — a greedy bot that always plays the move
// capturing the most opponent tiles in a single turn
void greedyMove(char board[][26], int n, char colour) {
  char opponent;
  if (colour == 'B') {
    opponent = 'W';
  } else {
    opponent = 'B';
  }

  int bestRow = -1;
  int bestCol = -1;
  int bestScore = -1;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (!isLegalMove(board, n, i, j, colour)) {
        continue;
      }

      // copy board into a scratch board
      char copy[26][26];
      for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
          copy[r][c] = board[r][c];
        }
      }

      // count opponent tiles before placing
      int before = 0;
      for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
          if (copy[r][c] == opponent) {
            before++;
          }
        }
      }

      placeTileAndFlip(copy, n, i, j, colour);

      // count opponent tiles after placing
      int after = 0;
      for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
          if (copy[r][c] == opponent) {
            after++;
          }
        }
      }

      int score = before - after;

      if (score > bestScore) {
        bestScore = score;
        bestRow = i;
        bestCol = j;
      }
    }
  }

  placeTileAndFlip(board, n, bestRow, bestCol, colour);
  printf("Computer places %c at %c%c.\n", colour, 'a' + bestRow, 'a' + bestCol);
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
    // check if neither player can move - game over
    if (!moveAvailable(board, n, 'B') && !moveAvailable(board, n, 'W')) {
      break;
    }

    // if current player has no move, skip their turn
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
      greedyMove(board, n, computerColour);
      printBoard(board, n);
    } else {
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