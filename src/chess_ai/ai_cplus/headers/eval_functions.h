#ifndef EVALUATION_FUNCTIONS_H
#define EVALUATION_FUNCTIONS_H

#include <array>
#include <eval_values.h>

// Function for summing the values of the pieces
float sum_piecevalues(std::array<std::array<int, 8>, 8> arr1, std::array<std::array<int, 8>, 8> arr2){
    float sum = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            sum += arr1[i][j] * arr2[j][i];
        }
    }
    return sum;
}

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

float evaluate_board(std::array<std::array<int, 8>, 8> board){
    float score = 0;

    // Get material values
    for (int i = -6; i <= 6; i++) {
        if (i == 0){continue;} // Skip 0 since there are no pieces called that
        std::array<std::array<int, 8>, 8> bool_board = bolean_board(board, i);
        std::array<std::array<int, 8>, 8> piece_values;

        // Get the piece values, depending on stage in game
        if(pieces_alive(board) > 22){
            piece_values = mg_value_tables.at(i);
        }else{
            piece_values = eg_value_tables.at(i);
        }

        score += sum_piecevalues(bool_board, piece_values);
    }

    // Get pawn values

    // Get King safety

    return score;
}

#endif