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
                    sum += value*mg_value_tables.at(value)[i][j]; // Piece value * the positional value
                    }
            }
        }
    }else{
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                value = board[i][j];
                if (value != 0){
                    sum += value*eg_value_tables.at(value)[i][j]; // Piece value * the positional value
                    }
            }
        }
    }
    
    return sum;
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

    // Get King safety
    

    return score;
}

#endif