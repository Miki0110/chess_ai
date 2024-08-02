#ifndef EVALUATION_H
#define EVALUATION_H

#include "ChessBoard.h"

class Evaluation {
public:
    static int evaluateBoard(const ChessBoard& board) {
        int score = 0;

        for (int pieceType = WHITE_PAWN; pieceType <= BLACK_KING; ++pieceType) {
            uint64_t pieceBitboard = board.getPieceBitboard(pieceType);
            int pieceCount = __builtin_popcountll(pieceBitboard);
            if (pieceType <= WHITE_KING) {
                score += pieceCount * pieceValues[pieceType];
            } else {
                score -= pieceCount * pieceValues[pieceType];
            }
        }

        return score;
    }

private:
    static const int pieceValues[12];
};

const int Evaluation::pieceValues[12] = {
    100, 320, 330, 500, 900, 20000,  // White pieces
    100, 320, 330, 500, 900, 20000   // Black pieces
};

#endif