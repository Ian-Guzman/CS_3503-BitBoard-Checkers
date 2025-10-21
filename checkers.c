
#include <stdio.h>
#include <stdlib.h>
#include "bitops64.h"   // 64 bit wrappers 

#define BOARD_SIZE 8
#define RED        'r'
#define BLACK      'b'
#define RED_KING   'R'
#define BLACK_KING 'B'
#define EMPTY      '.'

typedef unsigned long long U64;

typedef struct {
    U64 red;         // red pieces
    U64 black;       // black pieces
    U64 redKings;    // red kings
    U64 blackKings;  // black kings
    int turn;        // 0 = red to move, 1 = black to move
} GameState;

// Helpers 

static inline int CoordToBitIndex(int row, int col) { return row * 8 + col; }

static inline int InBounds(int r, int c) {
    return (r >= 0 && r < 8 && c >= 0 && c < 8);
}

static inline U64 AllPieces(const GameState *g) {
    return g->red | g->black | g->redKings | g->blackKings;
}

static inline int IsEmptySquare(const GameState *g, int idx) {
    return !GetBit64(AllPieces(g), idx);
}

// Display 

void PrintBoard(const GameState *g) {
    printf("    0 1 2 3 4 5 6 7\n");
    printf("   -----------------\n");
    for (int r = 0; r < 8; r++) {
        printf("%d | ", r);
        for (int c = 0; c < 8; c++) {
            int idx = CoordToBitIndex(r, c);
            if (GetBit64(g->redKings, idx))      { printf("%c ", RED_KING);   }
            else if (GetBit64(g->red, idx))      { printf("%c ", RED);        }
            else if (GetBit64(g->blackKings, idx)){ printf("%c ", BLACK_KING); }
            else if (GetBit64(g->black, idx))    { printf("%c ", BLACK);      }
            else                                  { printf("%c ", EMPTY);      }
        }
        printf("\n");
    }
    // piece count
    int redCount   = CountBits64(g->red) + CountBits64(g->redKings);
    int blackCount = CountBits64(g->black) + CountBits64(g->blackKings);
    printf("\nPieces Remaining:\n");
    printf("  Red:   %d\n", redCount);
    printf("  Black: %d\n", blackCount);
    printf("\n");
}

void InitializeBoard(GameState *g) {
    g->red = g->black = g->redKings = g->blackKings = 0ULL;
    g->turn = 0; // Red starts

    // Red on rows 0..2, dark squares only
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 8; c++) {
            if ((r + c) % 2 == 1) {
                int idx = CoordToBitIndex(r, c);
                g->red = SetBit64(g->red, idx);
            }
        }
    }
    // Black on rows 5..7, dark squares only
    for (int r = 5; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if ((r + c) % 2 == 1) {
                int idx = CoordToBitIndex(r, c);
                g->black = SetBit64(g->black, idx);
            }
        }
    }
}

// Validation

int IsValidMove(const GameState *g, int fromRow, int fromCol, int toRow, int toCol) {
    // bounds
    if (!InBounds(fromRow, fromCol) || !InBounds(toRow, toCol)) return 0;

    int fromIdx = CoordToBitIndex(fromRow, fromCol);
    int toIdx   = CoordToBitIndex(toRow, toCol);

    // detect mover piece
    int movingRedMan   = GetBit64(g->red, fromIdx);
    int movingRedKing  = GetBit64(g->redKings, fromIdx);
    int movingBlkMan   = GetBit64(g->black, fromIdx);
    int movingBlkKing  = GetBit64(g->blackKings, fromIdx);

    // must be current player's piece
    if (g->turn == 0) {
        if (!(movingRedMan || movingRedKing)) return 0;
    } else {
        if (!(movingBlkMan || movingBlkKing)) return 0;
    }

    // destination must be empty
    if (!IsEmptySquare(g, toIdx)) return 0;

    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;
    int ar = rowDiff < 0 ? -rowDiff : rowDiff;
    int ac = colDiff < 0 ? -colDiff : colDiff;

    int isKing = (g->turn == 0) ? movingRedKing : movingBlkKing;

    // simple move: diagonal by 1
    if (ar == 1 && ac == 1) {
        if (!isKing) {
            if (g->turn == 0 && rowDiff != 1)  return 0; // red moves down
            if (g->turn == 1 && rowDiff != -1) return 0; // black moves up
        }
        return 1;
    }

    // capture: diagonal by 2, middle must be opponent
    if (ar == 2 && ac == 2) {
        int midRow = (fromRow + toRow) / 2;
        int midCol = (fromCol + toCol) / 2;
        int midIdx = CoordToBitIndex(midRow, midCol);

        U64 enemy = (g->turn == 0)
                    ? (g->black | g->blackKings)
                    : (g->red   | g->redKings);

        if (!GetBit64(enemy, midIdx)) return 0; // must jump an enemy

        if (!isKing) {
            if (g->turn == 0 && rowDiff != 2)  return 0; // red jumps down
            if (g->turn == 1 && rowDiff != -2) return 0; // black jumps up
        }
        return 1;
    }

    return 0;
}

// Execute a move 

void MakeMove(GameState *g, int fromRow, int fromCol, int toRow, int toCol) {
    int fromIdx = CoordToBitIndex(fromRow, fromCol);
    int toIdx   = CoordToBitIndex(toRow, toCol);

    U64 *mPieces, *mKings, *ePieces, *eKings;

    if (g->turn == 0) 
    { // red
        mPieces = &g->red;      mKings = &g->redKings;
        ePieces = &g->black;    eKings = &g->blackKings;
    } else {
        mPieces = &g->black;    mKings = &g->blackKings;
        ePieces = &g->red;      eKings = &g->redKings;
    }

    int wasKing = GetBit64(*mKings, fromIdx);

    // clear source
    *mPieces = ClearBit64(*mPieces, fromIdx);
    *mKings  = ClearBit64(*mKings,  fromIdx);

    // capture
    if (abs(toRow - fromRow) == 2) {
        int mr = (fromRow + toRow) / 2;
        int mc = (fromCol + toCol) / 2;
        int midIdx = CoordToBitIndex(mr, mc);
        *ePieces = ClearBit64(*ePieces, midIdx);
        *eKings  = ClearBit64(*eKings,  midIdx);
    }

    // promotion
    int nowKing = wasKing;
    if (!nowKing) {
        if (g->turn == 0 && toRow == 7) nowKing = 1;
        if (g->turn == 1 && toRow == 0) nowKing = 1;
    }

    // place destination
    if (nowKing) {
        *mKings  = SetBit64(*mKings,  toIdx);
    } else {
        *mPieces = SetBit64(*mPieces, toIdx);
    }
}

// multi jump

int HasAnotherJump(const GameState *g, int row, int col) {
    // Check if the moved piece at (row,col) can jump again.
    int fromIdx = CoordToBitIndex(row, col);

    int isRedTurn = (g->turn == 0);
    int isKing = isRedTurn ? GetBit64(g->redKings, fromIdx)
                           : GetBit64(g->blackKings, fromIdx);

    // If moved piece no longer there (edge cases), no jumps.
    U64 myAll = isRedTurn ? (g->red | g->redKings) : (g->black | g->blackKings);
    if (!GetBit64(myAll, fromIdx)) return 0;

    int dirs[4][2] = { {2,2}, {2,-2}, {-2,2}, {-2,-2} };
    for (int i = 0; i < 4; i++) {
        int dr = dirs[i][0], dc = dirs[i][1];
        // restrict non-king direction
        if (!isKing) {
            if (isRedTurn && dr < 0)  continue; // red only down
            if (!isRedTurn && dr > 0) continue; // black only up
        }
        int tr = row + dr, tc = col + dc;
        if (!InBounds(tr, tc)) continue;

        int toIdx = CoordToBitIndex(tr, tc);
        if (!IsEmptySquare(g, toIdx)) continue;

        int mr = row + dr/2, mc = col + dc/2;
        int midIdx = CoordToBitIndex(mr, mc);

        U64 enemy = isRedTurn ? (g->black | g->blackKings) : (g->red | g->redKings);
        if (GetBit64(enemy, midIdx)) {
            // a legal jump exists
            return 1;
        }
    }
    return 0;
}

void HandleMultiJump(GameState *g, int row, int col) {
    // Keep auto-chaining jumps by the same piece while legal.
    while (HasAnotherJump(g, row, col)) {
        int made = 0;
        int dirs[4][2] = { {2,2}, {2,-2}, {-2,2}, {-2,-2} };
        for (int i = 0; i < 4 && !made; i++) {
            int tr = row + dirs[i][0];
            int tc = col + dirs[i][1];
            if (!InBounds(tr, tc)) continue;
            if (IsValidMove(g, row, col, tr, tc)) {
                MakeMove(g, row, col, tr, tc);
                row = tr; col = tc;
                made = 1;
            }
        }
    }
}

// Any move for player?

int HasAnyMoves(GameState *g, int forRed) {
    int originalTurn = g->turn;
    g->turn = forRed ? 0 : 1;
    U64 men   = forRed ? g->red   : g->black;
    U64 kings = forRed ? g->redKings : g->blackKings;


    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            int idx = CoordToBitIndex(r, c);
            int hasPiece = GetBit64(men, idx) || GetBit64(kings, idx);
            if (!hasPiece) continue;

            // try all diagonals at distance 1 and 2
            for (int dr = -2; dr <= 2; dr++) {
                for (int dc = -2; dc <= 2; dc++) {
                    int ar = dr < 0 ? -dr : dr;
                    int ac = dc < 0 ? -dc : dc;
                    if (ar == 0 || ar != ac) continue;

                    int tr = r + dr, tc = c + dc;
                    if (!InBounds(tr, tc)) continue;

                    if (IsValidMove(g, r, c, tr, tc))
                    {
                         g->turn = originalTurn; // restore before returning
                        return 1;
                    }
                }
            }
        }
    }
    // Restore original turn before returning
    g->turn = originalTurn;
    return 0;
}



void PlayTurn(GameState *g) {
    int fromRow, fromCol, toRow, toCol;

    printf("%s's turn\n", g->turn == 0 ? "Red" : "Black");
    printf("Enter move (fromRow fromCol toRow toCol): ");

    if (scanf("%d %d %d %d", &fromRow, &fromCol, &toRow, &toCol) != 4) {
        printf("Invalid input. Try again.\n");
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
        return;
    }

    if (IsValidMove(g, fromRow, fromCol, toRow, toCol)) {
        int wasJump = (abs(toRow - fromRow) == 2);
        MakeMove(g, fromRow, fromCol, toRow, toCol);

        if (wasJump) {
            // auto-chain jumps with the same piece
            HandleMultiJump(g, toRow, toCol);
        }

        // switch turns after any chained jumps
        g->turn ^= 1;
    } else {
        printf("Invalid move. Try again.\n");
    }
}

// Main

int main(void) {
    GameState g;
    InitializeBoard(&g);

    while (1) {
        PrintBoard(&g);

        // if current player cannot move, other side wins
        int currentIsRed = (g.turn == 0);
        if (!HasAnyMoves(&g, currentIsRed)) {
            printf("%s has no moves. %s wins!\n",
                   currentIsRed ? "Red" : "Black",
                   currentIsRed ? "Black" : "Red");
            break;
        }

        PlayTurn(&g);
    }

    return 0;
}