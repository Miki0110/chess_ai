#ifndef MINIMAX_H
#define MINIMAX_H

#include "ChessBoard.h"
#include "MoveGenerator.h"
#include "Evaluation.h"
#include "TranspositionTable.h"
#include <limits>
#include <unordered_map>
#include <algorithm>
#include <future>  // For std::async
#include <vector>
#include <mutex>



struct MinimaxResult
{
    int value;
    Move move;
    std::unordered_map<uint64_t, int> transpositionTable;
};


class Minimax {
public:
    static Move findBestMove(const ChessBoard& board, int depth, bool whiteToMove) {
        int bestValue = whiteToMove ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        Move bestMove(-1, -1, -1, -1);

        auto& transpositionTable = TranspositionTable::getInstance();  // Get reference to the global table
        auto& tableMutex = TranspositionTable::getMutex();  // Shared mutex for thread-safe access to the global transposition table

        // Generate all legal moves for the current position
        ChessBoard currentBoard = board;
        auto moves = MoveGenerator::generateLegalMoves(currentBoard, whiteToMove);

        // Use multithreading to evaluate each move in parallel
        std::vector<std::future<std::tuple<Move, int, std::unordered_map<uint64_t, int>>>> futures;  // Store future results

        for (const Move& move : moves) {
            // Launch each move evaluation in parallel using std::async
            futures.push_back(std::async(std::launch::async, [move, board = currentBoard, depth, whiteToMove, &transpositionTable]() mutable {
                // Initialize alpha and beta
                int alpha = std::numeric_limits<int>::min();
                int beta = std::numeric_limits<int>::max();
                
                // Local transposition table for this thread
                std::unordered_map<uint64_t, int> localTable;

                board.applyMove(move);  // Work with the copied board
                int value = minimax(board, depth - 1, !whiteToMove, alpha, beta, localTable, transpositionTable);

                std::cout << "Move evaluated: " << convertMoveToNotation(move) << " Value: " << value << std::endl;
                board.undoMove(move);

                // Return the value and the local transposition table
                return std::make_tuple(move, value, localTable);
            }));
        }

        // Wait for all the results and find the best move, while also merging the transposition tables
        std::cout << "Waiting for result..." << std::endl;
        for (auto& future : futures) {
            auto result = future.get();  // Get the result from the thread
            Move move = std::get<0>(result);
            int boardValue = std::get<1>(result);
            std::unordered_map<uint64_t, int> localTable = std::get<2>(result);

            // Merge the local table into the global table in a thread-safe way
            {
                std::lock_guard<std::mutex> lock(tableMutex);
                for (const auto& entry : localTable) {
                    transpositionTable[entry.first] = entry.second;  // Insert or update the key-value pair
                }
            }

            // Update best move if needed
            if (whiteToMove && boardValue > bestValue) {
                bestValue = boardValue;
                bestMove = move;
            } else if (!whiteToMove && boardValue < bestValue) {
                bestValue = boardValue;
                bestMove = move;
            }
        }

        std::cout << "Best move: " << convertMoveToNotation(bestMove) << std::endl;
        return bestMove;
    }

private:
    // minimax function using a local transposition table
    static int minimax(ChessBoard& board, int depth, bool whiteToMove, int alpha, int beta, std::unordered_map<uint64_t, int>& localTable, const std::unordered_map<uint64_t, int>& globalTable) {
        if (depth == 0) {
            return Evaluation::evaluateBoard(board);
        }

        uint64_t hash = board.hash();  // Calculate the Zobrist hash for the current board state

        {
            std::lock_guard<std::mutex> lock(TranspositionTable::getMutex());
            if (globalTable.find(hash) != globalTable.end()) {
                return globalTable.at(hash);  // Return cached value
            }
        }

        // Check if the current position is in the local table (useful for the same recursion call)
        if (localTable.find(hash) != localTable.end()) {
            return localTable[hash];  // Return the cached evaluation from the local table
        }

        auto moves = MoveGenerator::generateLegalMoves(board, whiteToMove);
        std::sort(moves.begin(), moves.end(), [&board, whiteToMove](const Move& a, const Move& b) {
            bool aCapture = board.isCapture(a, whiteToMove);
            bool bCapture = board.isCapture(b, whiteToMove);
            return aCapture > bCapture;  // Prioritize capture moves
        });

        if (moves.empty()) {
            return Evaluation::evaluateBoard(board);
        }

        int value;
        if (whiteToMove) {
            value = std::numeric_limits<int>::min();
            for (const Move& move : moves) {
                board.applyMove(move);
                int eval = minimax(board, depth - 1, false, alpha, beta, localTable, globalTable);
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
            for (const Move& move : moves) {
                board.applyMove(move);
                int eval = minimax(board, depth - 1, true, alpha, beta, localTable, globalTable);
                board.undoMove(move);

                value = std::min(value, eval);
                beta = std::min(beta, eval);

                // Alpha-Beta Pruning
                if (beta <= alpha) {
                    break;
                }
            }
        }

        // Store the result in the local transposition table
        localTable[hash] = value;
        return value;
    }
};

#endif
