#ifndef MINIMAX_H
#define MINIMAX_H

#include "ChessBoard.h"
#include "MoveGenerator.h"
#include "Evaluation.h"
#include <limits>
#include <unordered_map>

class Minimax {
public:
    static Move findBestMove(const ChessBoard& board, int depth, bool whiteToMove) {
        int bestValue = whiteToMove ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        Move bestMove(-1, -1, -1, -1);

        // Generate all legal moves for the current position
        ChessBoard currentBoard = board;
        auto moves = MoveGenerator::generateLegalMoves(currentBoard, whiteToMove);

        // Initialize alpha and beta
        int alpha = std::numeric_limits<int>::min();
        int beta = std::numeric_limits<int>::max();

        // Evaluate each move using minimax algorithm with alpha-beta pruning and hashing
        for (const Move& move : moves) {
            currentBoard.applyMove(move);

            int boardValue = minimax(currentBoard, depth - 1, !whiteToMove, alpha, beta, whiteToMove);

            currentBoard.undoMove(move);

            // Update best move if needed
            if (whiteToMove && boardValue > bestValue) {
                bestValue = boardValue;
                bestMove = move;
                alpha = std::max(alpha, bestValue);
            } else if (!whiteToMove && boardValue < bestValue) {
                bestValue = boardValue;
                bestMove = move;
                beta = std::min(beta, bestValue);
            }

            // Alpha-Beta Pruning: Stop further evaluation if we found a better move for opponent
            if (beta <= alpha) {
                break;
            }
        }

        return bestMove;
    }

private:
    static std::unordered_map<uint64_t, int> transpositionTable; // Transposition table

    static int minimax(ChessBoard& board, int depth, bool maximizingPlayer, int alpha, int beta, bool whiteToMove) {
        if (depth == 0) {
            return Evaluation::evaluateBoard(board);
        }

        uint64_t hash = board.hash(); // Calculate the Zobrist hash for the current board state

        // Check if the current position is already evaluated
        if (transpositionTable.find(hash) != transpositionTable.end()) {
            return transpositionTable[hash]; // Return the cached evaluation
        }

        int value;
        if (maximizingPlayer) {
            value = std::numeric_limits<int>::min();
            auto moves = MoveGenerator::generateLegalMoves(board, whiteToMove);

            for (const Move& move : moves) {
                board.applyMove(move);

                int eval = minimax(board, depth - 1, false, alpha, beta, !whiteToMove);

                board.undoMove(move);

                value = std::max(value, eval);
                alpha = std::max(alpha, eval);

                // Alpha-Beta Pruning
                if (beta <= alpha) {
                    break;
                }
            }
        } else {
            value = std::numeric_limits<int>::max();
            auto moves = MoveGenerator::generateLegalMoves(board, !whiteToMove);

            for (const Move& move : moves) {
                board.applyMove(move);

                int eval = minimax(board, depth - 1, true, alpha, beta, !whiteToMove);

                board.undoMove(move);

                value = std::min(value, eval);
                beta = std::min(beta, eval);

                // Alpha-Beta Pruning
                if (beta <= alpha) {
                    break;
                }
            }
        }

        transpositionTable[hash] = value; // Store the evaluated value in the transposition table
        return value;
    }
};

// Define the static member outside the class
std::unordered_map<uint64_t, int> Minimax::transpositionTable;

#endif