#ifndef SEARCH_ALGORITHMS_H
#define SEARCH_ALGORITHMS_H

#include <array>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <iostream>
#include <chrono> // For time tracking
#include <board_representation.h> // Ensure this includes necessary board logic

struct MiniMaxResult {
    int score;
    std::array<int, 4> move;
};

// Function to print progress every 5 seconds
void printProgress(std::chrono::steady_clock::time_point start_time, int depth, const std::array<int, 4>& best_move, int best_score) {
    static std::chrono::steady_clock::time_point last_print_time = start_time;
    auto current_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = current_time - last_print_time;

    if (elapsed_seconds.count() >= 5.0) { // Every 5 seconds
        last_print_time = current_time;
        std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count()
                  << " seconds | Depth: " << depth << " | Best Move: " << best_move[0] << "," << best_move[1] << "," << best_move[2] << "," << best_move[3]
                  << " | Best Score: " << best_score << std::endl;
    }
}

MiniMaxResult minimax(int depth, Board *board, int alpha, int beta, std::unordered_map<std::string, MiniMaxResult> &hash_table, bool maximizing_player, std::chrono::steady_clock::time_point start_time) {
    int side = maximizing_player ? 1 : -1;

    // Check if the board state has already been evaluated and stored in the hash table
    std::string board_key = board->board_to_fen(side);
    if (hash_table.find(board_key) != hash_table.end()) {
        return hash_table[board_key];
    }

    // Terminal node or depth limit reached
    if (depth == 0 || board->is_game_over()) {
        int score = board->get_board_value();
        return {score, {-1, -1, -1, -1}};
    }

    std::array<int, 4> best_move = {-1, -1, -1, -1};
    int best_score = maximizing_player ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    // Generate all possible moves
    std::vector<std::array<int, 4>> possible_moves = board->get_allmoves(side);

    // Print progress every 5 seconds
    printProgress(start_time, depth, best_move, best_score);

    // No moves available means checkmate or stalemate
    if (possible_moves.empty()) {
        int score = board->get_board_value();
        return {score, {-1, -1, -1, -1}};
    }

    for (const auto& move : possible_moves) {
        // Move the piece
        board->move_piece(move[0], move[1], move[2], move[3]);

        // Recursively call MiniMax
        MiniMaxResult result = minimax(depth - 1, board, alpha, beta, hash_table, !maximizing_player, start_time);

        // Undo the move
        board->undo_move();

        if (maximizing_player) {
            // Update best score and move if the current score is better
            if (result.score > best_score) {
                best_score = result.score;
                best_move = move;
            }
            alpha = std::max(alpha, best_score);
        } else {
            // Update best score and move if the current score is better
            if (result.score < best_score) {
                best_score = result.score;
                best_move = move;
            }
            beta = std::min(beta, best_score);
        }

        // Alpha-beta pruning
        if (beta <= alpha) {
            break;
        }

        
    }

    // Store the board state and its score in the hash table
    hash_table[board_key] = {best_score, best_move};
    return {best_score, best_move};
}

MiniMaxResult start_minimax(int depth, Board *board, bool maximizing_player) {
    std::unordered_map<std::string, MiniMaxResult> hash_table;
    auto start_time = std::chrono::steady_clock::now();

    // Start MiniMax
    MiniMaxResult result = minimax(depth, board, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), hash_table, maximizing_player, start_time);

    // Print final best move and score
    std::cout << "Best move: " << result.move[0] << "," << result.move[1] << "," << result.move[2] << "," << result.move[3] << std::endl;

    return result;
}

#endif
