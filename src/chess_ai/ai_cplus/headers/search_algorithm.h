#ifndef SEARCH_ALGORITHMS_H
#define SEARCH_ALGORITHMS_H

#include <array>
#include <unordered_map>
#include <board_representation.h>
#include <cmath>
#include <limits>

struct MiniMaxResult {
    int score;
    std::array<int, 4> move;
};


// Todo implement hashtable
MiniMaxResult minimax(int depth, Board *board, int alpha, int beta, std::unordered_map<std::string, MiniMaxResult> &hash_table, bool maximizing_player) {
    // Check if the board state has already been evaluated and stored in the hash table

    int side = maximizing_player ? 1 : -1;

    // The FEN key is used to see save positions in the hashtable
    std::string board_key = board->board_to_fen(side);
    if (hash_table.find(board_key) != hash_table.end()) {
        return hash_table[board_key];
    }

    // When we've found the best solution
    if (depth == 0 || board->is_game_over()) {
        int score = board->get_board_value();
        return {score, { -1, -1, -1, -1 }};
    }

    std::array<int, 4> best_move = { -1, -1, -1, -1 };

    if (maximizing_player) {
        // Generate all possible moves
        std::vector<std::array<int, 4>> possible_moves = board->get_allmoves(1);
        // Initiate max score as a low value
        int max_score = std::numeric_limits<int>::min();
        // When there are no moves it's either a mate or the king is dead
        if (possible_moves.size() == 0) {
            int score = board->get_board_value();
            return {score, {-1, -1, -1, -1}};
        }
        
        for (int i = 0; i < possible_moves.size(); i++) {
            std::array<int, 4> move = possible_moves[i];
            // Move the piece
            board->move_piece(move[0], move[1], move[2], move[3]);
            // Repeat the process
            MiniMaxResult result = minimax(depth - 1, board, alpha, beta, hash_table, false);
            // Undo the move we just did
            board->undo_move();

            // Set the scores
            if (max_score < result.score) {
                max_score = result.score;
                best_move = {move[0], move[1], move[2], move[3]};  
            }
            alpha = std::max(alpha, max_score);
            // Prune if the alpha is bigger than beta
            if (beta <= alpha) {
                break;
            }
        }

        // Store the board state and its score in the hash table
        hash_table[board_key] = {max_score, best_move};
        return {max_score, best_move};
    } else {
        // Initiate max score as a low value
        int min_score = std::numeric_limits<int>::max();
        // Generate all possible moves
        std::vector<std::array<int, 4>> possible_moves = board->get_allmoves(-1);
        // When there are no moves it's either a mate or the king is dead
        if (possible_moves.size() == 0) {
            int score = board->get_board_value();
            return {score, {-1, -1, -1, -1}};
        }
        for (int i = 0; i < possible_moves.size(); i++) {
            std::array<int, 4> move = possible_moves[i];
            // Move the piece
            board->move_piece(move[0], move[1], move[2], move[3]);
            // Repeat the process
            MiniMaxResult result = minimax(depth - 1, board, alpha, beta, hash_table, true);
            // Undo the move we just did
            board->undo_move();
            // Set the scores
            if (min_score >= result.score) {
                min_score = result.score;
                best_move = {move[0], move[1], move[2], move[3]};
            }
            beta = std::min(beta, min_score);
            if (beta <= alpha) {
                break;
            }
        }

        // Store the board state and its score in the hash table
        hash_table[board_key] = {min_score, best_move};
        return {min_score, best_move};
    }
}


MiniMaxResult start_minimax(int depth, Board *board, bool maximizing_player) {
    std::unordered_map<std::string, MiniMaxResult> hash_table = {};
    std::cout << "Hash table size: " << hash_table.size() << std::endl;  
    
    MiniMaxResult result = minimax(depth, board, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), hash_table, maximizing_player);

    // DEBUGGING
    std::cout << "Hash table size: " << hash_table.size() << std::endl;  
    
    return result;
}

#endif