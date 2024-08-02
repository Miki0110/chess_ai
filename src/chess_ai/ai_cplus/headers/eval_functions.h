#ifndef EVALUATION_FUNCTIONS_H
#define EVALUATION_FUNCTIONS_H

#include <array>
#include <eval_values.h>

int pieces_alive(std::array<std::array<int, 8>, 8> board){
    int pieces = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != 0) {
                pieces++;
            }
        }
    }
    return pieces;
}

// Function for summing the values of the pieces
int sum_materialvalues(std::array<std::array<int, 8>, 8> board){
    int sum = 0;
    int value = 0;

    // Get the piece values, depending on stage in game
    if(pieces_alive(board) > 22){
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                value = board[i][j];
                if (value != 0){
                    sum += piece_value.at(value)+mg_value_tables.at(value)[i][j]; // Piece value * the positional value
                    }
            }
        }
    }else{
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                value = board[i][j];
                if (value != 0){
                    sum += piece_value.at(value)+eg_value_tables.at(value)[i][j]; // Piece value * the positional value
                    }
            }
        }
    }
    
    return sum;
}

// Pawn structure evaluation
int evaluate_pawn_structure(std::array<std::array<int, 8>, 8> board) {
    int score = 0;

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int piece = board[rank][file];
            if (abs(piece) == 1) { // Check if it's a pawn
                int side = piece > 0 ? 1 : -1;

                // Check for doubled pawns
                for (int r = rank + side; r >= 0 && r < 8; r += side) {
                    if (board[r][file] == piece) {
                        score -= side * 10; // Penalize doubled pawns
                        break;
                    }
                }

                // Check for isolated pawns
                bool isolated = true;
                if (file > 0) {
                    for (int r = 0; r < 8; ++r) {
                        if (board[r][file - 1] == piece) {
                            isolated = false;
                            break;
                        }
                    }
                }
                if (file < 7 && isolated) {
                    for (int r = 0; r < 8; ++r) {
                        if (board[r][file + 1] == piece) {
                            isolated = false;
                            break;
                        }
                    }
                }
                if (isolated) {
                    score -= side * 20; // Penalize isolated pawns
                }

                // Check for passed pawns (Free movement)
                bool passed = true;
                for (int r = rank + side; r >= 0 && r < 8 && passed; r += side) {
                    for (int f = std::max(0, file - 1); f <= std::min(7, file + 1); ++f) {
                        if (board[r][f] == -piece) {
                            passed = false;
                            break;
                        }
                    }
                }
                if (passed) {
                    score += side * 30; // Reward passed pawns
                }
            }
        }
    }

    return score;
}

int evaluate_king_safety(std::array<std::array<int, 8>, 8> board, int king_pos[2][2]) {
    int score = 0;
    int directions[8][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    int knight_moves[8][2] = {{1, 2}, {2, 1}, {-1, 2}, {-2, 1}, {1, -2}, {2, -1}, {-1, -2}, {-2, -1}};

    for (int color = 0; color < 2; color++) {
        int king_row = king_pos[color][0];
        int king_col = king_pos[color][1];
        int side = color == 0 ? 1 : -1;

        if (king_row == -1) { // The king is dead
            continue;
        }

        int attack_score = 0;

        // Check for attackers in each direction (rook, bishop, queen)
        for (int i = 0; i < 8; i++) {
            int row = king_row + directions[i][0];
            int col = king_col + directions[i][1];

            while (row >= 0 && row < 8 && col >= 0 && col < 8) {
                int piece = board[row][col];

                if (piece != 0) {
                    if (piece * side < 0) { // If it's an enemy piece
                        // Rook or queen
                        if (abs(piece) == 4 && (i < 4)) {
                            attack_score += 5;
                        }
                        // Bishop or queen
                        else if (abs(piece) == 3 && (i >= 4)) {
                            attack_score += 3;
                        }
                    }
                    break;
                }

                row += directions[i][0];
                col += directions[i][1];
            }
        }

        // Check for knight attacks
        for (int i = 0; i < 8; i++) {
            int row = king_row + knight_moves[i][0];
            int col = king_col + knight_moves[i][1];

            if (row >= 0 && row < 8 && col >= 0 && col < 8) {
                int piece = board[row][col];
                if (piece * side < 0 && abs(piece) == 2) { // If it's an enemy knight
                    attack_score += 3;
                }
            }
        }

        // Check for pawn attacks
        int pawn_directions[2][2] = {{1, -1}, {1, 1}};
        for (int i = 0; i < 2; i++) {
            int row = king_row + side * pawn_directions[i][0];
            int col = king_col + pawn_directions[i][1];

            if (row >= 0 && row < 8 && col >= 0 && col < 8) {
                int piece = board[row][col];
                if (piece * side < 0 && abs(piece) == 1) { // If it's an enemy pawn
                    attack_score += 1;
                }
            }
        }

        score -= side * attack_score;
                // Check for pieces surrounding the king
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) {
                    continue;
                }

                int row = king_row + i;
                int col = king_col + j;

                if (row >= 0 && row < 8 && col >= 0 && col < 8) {
                    int piece = board[row][col];
                    if (piece * side > 0) { // If it's an ally piece
                        attack_score -= 2;
                    }
                }
            }
        }

        score -= side * attack_score;
    }

    return score;
}

// Function for getting the bollean board
std::array<std::array<int, 8>, 8> bolean_board(std::array<std::array<int, 8>, 8> board, int value){
    std::array<std::array<int, 8>, 8> new_arr {};

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == value) {
                if (value > 0) {
                    new_arr[i][j] = 1;
                }else{
                    new_arr[i][j] = -1;
                }
            }
        }
    }
    return new_arr;
}

float evaluate_board(std::array<std::array<int, 8>, 8> board, int king_pos[2][2]){
    float score = 0;

    // Get material values
    score += sum_materialvalues(board);

    // Get pawn values
    score += evaluate_pawn_structure(board);


    // Get King safety
    score += evaluate_king_safety(board, king_pos);
    

    return score;
}

#endif