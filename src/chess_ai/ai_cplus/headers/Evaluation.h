#ifndef EVALUATION_H
#define EVALUATION_H

#include "ChessBoard.h"
#include <array>
#include <bitset>

#define FLIP(sq) ((sq)^56)

// Board size constant
constexpr int BOARD_SIZE = 64;


class Evaluation {
public:
    static int evaluateBoard(const ChessBoard& board) {
        int score = 0;

        // Evaluate material balance
        score += evaluateMaterial(board);

        // Evaluate piece positions using piece-square tables
        score += evaluatePiecePositions(board);

        // Evaluate King safety
        //score += evaluateKingSafety(board);

        return score;
    }

//private:
    // Material values for each piece type
    static const int pieceValues[12];

    // Piece-square tables
    static const std::array<int, BOARD_SIZE> mgPawnTable;
    static const std::array<int, BOARD_SIZE> egPawnTable;
    static const std::array<int, BOARD_SIZE> mgKnightTable;
    static const std::array<int, BOARD_SIZE> egKnightTable;
    static const std::array<int, BOARD_SIZE> mgBishopTable;
    static const std::array<int, BOARD_SIZE> egBishopTable;
    static const std::array<int, BOARD_SIZE> mgRookTable;
    static const std::array<int, BOARD_SIZE> egRookTable;
    static const std::array<int, BOARD_SIZE> mgQueenTable;
    static const std::array<int, BOARD_SIZE> egQueenTable;
    static const std::array<int, BOARD_SIZE> mgKingTable;
    static const std::array<int, BOARD_SIZE> egKingTable;
    


    static int calculateGamePhase(const ChessBoard& board) {
        // Define weights for each piece type
        const int knightWeight = 1;
        const int bishopWeight = 1;
        const int rookWeight = 2;
        const int queenWeight = 4;
        
        int phase = 0;
        // Calculate phase value based on remaining material
        phase += __builtin_popcountll(board.getPieceBitboard(WHITE_KNIGHT)) * knightWeight;
        phase += __builtin_popcountll(board.getPieceBitboard(WHITE_BISHOP)) * bishopWeight;
        phase += __builtin_popcountll(board.getPieceBitboard(WHITE_ROOK)) * rookWeight;
        phase += __builtin_popcountll(board.getPieceBitboard(WHITE_QUEEN)) * queenWeight;

        phase += __builtin_popcountll(board.getPieceBitboard(BLACK_KNIGHT)) * knightWeight;
        phase += __builtin_popcountll(board.getPieceBitboard(BLACK_BISHOP)) * bishopWeight;
        phase += __builtin_popcountll(board.getPieceBitboard(BLACK_ROOK)) * rookWeight;
        phase += __builtin_popcountll(board.getPieceBitboard(BLACK_QUEEN)) * queenWeight;

        return phase;
    }

    // Evaluate material balance
    static int evaluateMaterial(const ChessBoard& board) {
        int materialScore = 0;

        for (int pieceType = WHITE_PAWN; pieceType <= BLACK_KING; ++pieceType) {
            uint64_t pieceBitboard = board.getPieceBitboard(pieceType);
            int pieceCount = __builtin_popcountll(pieceBitboard);
            if (pieceType <= WHITE_KING) {
                materialScore += pieceCount * pieceValues[pieceType];
            } else {
                materialScore -= pieceCount * pieceValues[pieceType];
            }
        }

        return materialScore;
    }

    // Evaluate material balance
    static int evaluatePiecePositions(const ChessBoard& board) {
        int mgPositionScore = 0; // Middle game score
        int egPositionScore = 0; // Endgame score

        // Iterate over each piece on the board
        for (int square = 0; square < BOARD_SIZE; ++square) {
            int pieceType = board.getPieceOnSquare(square);
            if (pieceType >= 0) {
                // Get piece-square values for both phases
                if (pieceType <= WHITE_KING) {
                    mgPositionScore += getPieceSquareValue(pieceType, square, true);
                    egPositionScore += getPieceSquareValue(pieceType, square, false);
                } else {
                    mgPositionScore -= getPieceSquareValue(pieceType, square, true); // Middle game
                    egPositionScore -= getPieceSquareValue(pieceType, square, false); // Endgame
                }
            }
        }

        int phase = calculateGamePhase(board);

        // Blend the scores based on the game phase
        const int MAX_PHASE = 24; // Max phase value at game start
        double phaseWeight = phase / static_cast<double>(MAX_PHASE);
        int taperedScore = mgPositionScore * phaseWeight + egPositionScore * (1.0 - phaseWeight);

        return taperedScore;
    }

    // Updated getPieceSquareValue function
    static int getPieceSquareValue(int pieceType, int square, bool isMiddleGame) {
        switch (pieceType) {
            case WHITE_PAWN: return isMiddleGame ? mgPawnTable[square] : egPawnTable[square];
            case WHITE_KNIGHT: return isMiddleGame ? mgKnightTable[square] : egKnightTable[square];
            case WHITE_BISHOP: return isMiddleGame ? mgBishopTable[square] : egBishopTable[square];
            case WHITE_ROOK: return isMiddleGame ? mgRookTable[square] : egRookTable[square];
            case WHITE_QUEEN: return isMiddleGame ? mgQueenTable[square] : egQueenTable[square];
            case WHITE_KING: return isMiddleGame ? mgKingTable[square] : egKingTable[square];
            case BLACK_PAWN: return isMiddleGame ? mgPawnTable[FLIP(square)] : egPawnTable[FLIP(square)];
            case BLACK_KNIGHT: return isMiddleGame ? mgKnightTable[FLIP(square)] : egKnightTable[FLIP(square)];
            case BLACK_BISHOP: return isMiddleGame ? mgBishopTable[FLIP(square)] : egBishopTable[FLIP(square)];
            case BLACK_ROOK: return isMiddleGame ? mgRookTable[FLIP(square)] : egRookTable[FLIP(square)];
            case BLACK_QUEEN: return isMiddleGame ? mgQueenTable[FLIP(square)] : egQueenTable[FLIP(square)];
            case BLACK_KING: return isMiddleGame ? mgKingTable[FLIP(square)] : egKingTable[FLIP(square)];
            default: return 0;
        }
    }

    // Evaluate pawn structure
    static int evaluatePawnStructure(const ChessBoard& board) {
        int pawnStructureScore = 0;
        // Implement pawn structure evaluation here...
        return pawnStructureScore;
    }

    // Evaluate mobility (number of legal moves)
    static int evaluateMobility(const ChessBoard& board) {
        int mobilityScore = 0;
        // Implement mobility evaluation here...
        return mobilityScore;
    }

    static int evaluateKingSafety(const ChessBoard& board) {
        int kingSafetyScore = 0;

        // Evaluate white king safety
        uint64_t whiteKingBitboard = board.getPieceBitboard(WHITE_KING);
        int whiteKingSquare = __builtin_ctzll(whiteKingBitboard); // Get the position of the white king
        kingSafetyScore += evaluateIndividualKingSafety(board, whiteKingSquare, true);

        // Evaluate black king safety
        uint64_t blackKingBitboard = board.getPieceBitboard(BLACK_KING);
        int blackKingSquare = __builtin_ctzll(blackKingBitboard); // Get the position of the black king
        kingSafetyScore -= evaluateIndividualKingSafety(board, blackKingSquare, false);

        return kingSafetyScore;
    }

    // Evaluate individual king safety
    static int evaluateIndividualKingSafety(const ChessBoard& board, int kingSquare, bool isWhite) {
        int score = 0;

        // Calculate pawn shield
        score += evaluatePawnShield(board, kingSquare, isWhite);

        // Count enemy attacks
        score += countEnemyAttacks(board, kingSquare, isWhite);

        return score;
    }

    // Evaluate the pawn shield in front of the king
    static int evaluatePawnShield(const ChessBoard& board, int kingSquare, bool isWhite) {
        int shieldScore = 0;

        int pawnType = isWhite ? WHITE_PAWN : BLACK_PAWN;
        int rankOffset = isWhite ? 8 : -8; // Direction for checking pawns
        int pawnShieldSquares[] = {kingSquare + rankOffset - 1, kingSquare + rankOffset, kingSquare + rankOffset + 1};

        for (int square : pawnShieldSquares) {
            if (square >= 0 && square < BOARD_SIZE) {
                if (board.getPieceOnSquare(square) == pawnType) {
                    shieldScore += 10; // Example score for pawn in shield
                } else {
                    shieldScore -= 20; // Penalty for missing pawn in shield
                }
            }
        }

        return shieldScore;
    }

    // Count enemy attacks around the king
    static int countEnemyAttacks(const ChessBoard& board, int kingSquare, bool isWhite) {
        int attackScore = 0;
        std::vector<int> directions = {1, -1, 8, -8, 9, -9, 7, -7}; // Directions: horizontal, vertical, diagonal

        for (int dir : directions) {
            int currentSquare = kingSquare + dir;
            while (currentSquare >= 0 && currentSquare < BOARD_SIZE) {
                int pieceType = board.getPieceOnSquare(currentSquare);
                if (pieceType != -1) {
                    if ((isWhite && pieceType >= BLACK_PAWN) || (!isWhite && pieceType <= WHITE_KING)) {
                        attackScore -= 5; // Penalty for enemy attacks
                    }
                    break;
                }
                currentSquare += dir;
            }
        }

        return attackScore;
    }

    // Evaluate center control
    static int evaluateCenterControl(const ChessBoard& board) {
        int centerControlScore = 0;
        // Implement center control evaluation here...
        return centerControlScore;
    }
};

// Material values for each piece type
const int Evaluation::pieceValues[12] = {
     10,  32,     33,     50,   90,    20000,  // White pieces
     10,  32,     33,     50,   90,    20000   // Black pieces
};//Pawn, Knight, Bishop, Rook, Queen, King


// Piece-square tables
const std::array<int, BOARD_SIZE> Evaluation::mgPawnTable = {
     0,   0,   0,   0,   0,   0,   0,   0,
     5,  10,  10, -20, -20,  10,  10,   5,
     5,  -5, -10,   0,   0, -10,  -5,   5,
     0,   0,   0,  20,  20,   0,   0,   0,
     5,   5,  10,  25,  25,  10,   5,   5,
    10,  10,  20,  30,  30,  20,  10,  10,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,   0,   0,   0,   0,   0,   0
};

const std::array<int, BOARD_SIZE> Evaluation::egPawnTable = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0
};

const std::array<int, BOARD_SIZE> Evaluation::mgKnightTable = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0
};

const std::array<int, BOARD_SIZE> Evaluation::egKnightTable = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23
};

const std::array<int, BOARD_SIZE> Evaluation::mgBishopTable = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21
};

const std::array<int, BOARD_SIZE> Evaluation::egBishopTable = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17
};

const std::array<int, BOARD_SIZE> Evaluation::mgRookTable = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26
};

const std::array<int, BOARD_SIZE> Evaluation::egRookTable = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20
};

const std::array<int, BOARD_SIZE> Evaluation::mgQueenTable = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50
};

const std::array<int, BOARD_SIZE> Evaluation::egQueenTable = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41
};

const std::array<int, BOARD_SIZE> Evaluation::mgKingTable = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14
};

const std::array<int, BOARD_SIZE> Evaluation::egKingTable = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};


#endif
