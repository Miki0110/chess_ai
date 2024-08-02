#ifndef MINIMAX_H
#define MINIMAX_H

#include "ChessBoard.h"
#include "MoveGenerator.h"
#include "Evaluation.h"
#include <limits>

class Minimax {
public:
    static Move findBestMove(const ChessBoard& board, int depth, bool whiteToMove) {
        int bestValue = whiteToMove ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        Move bestMove(-1, -1, -1, -1);

        ChessBoard currentBoard = board;
        auto moves = MoveGenerator::generateLegalMoves(currentBoard, whiteToMove);

        for (const Move& move : moves) {
            currentBoard.applyMove(move);

            int boardValue = minimax(currentBoard, depth - 1, !whiteToMove, whiteToMove);

            currentBoard.undoMove(move);

            if (whiteToMove && boardValue > bestValue) {
                bestValue = boardValue;
                bestMove = move;
            } else if (!whiteToMove && boardValue < bestValue) {
                bestValue = boardValue;
                bestMove = move;
            }
        }

        return bestMove;
    }

private:
    static int minimax(ChessBoard& board, int depth, bool maximizingPlayer, bool whiteToMove) {
        if (depth == 0) {
            return Evaluation::evaluateBoard(board);
        }

        if (maximizingPlayer) {
            int maxEval = std::numeric_limits<int>::min();
            auto moves = MoveGenerator::generateLegalMoves(board, whiteToMove);

            for (const Move& move : moves) {
                board.applyMove(move);

                int eval = minimax(board, depth - 1, false, !whiteToMove);

                board.undoMove(move);

                maxEval = std::max(maxEval, eval);
            }
            return maxEval;
        } else {
            int minEval = std::numeric_limits<int>::max();
            auto moves = MoveGenerator::generateLegalMoves(board, !whiteToMove);

            for (const Move& move : moves) {
                board.applyMove(move);

                int eval = minimax(board, depth - 1, true, !whiteToMove);

                board.undoMove(move);

                minEval = std::min(minEval, eval);
            }
            return minEval;
        }
    }
};

#endif