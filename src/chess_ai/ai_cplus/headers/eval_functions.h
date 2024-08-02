#ifndef EVALUATION_FUNCTIONS_H
#define EVALUATION_FUNCTIONS_H

#include <array>
#include <eval_values.h>

// Function for summing the values of the pieces
int sum_material_values(std::array<std::array<int, 8>, 8> board, int pieces_alive){
    int sum = 0;
    int value = 0;

    // Get the piece values, depending on stage in game
    if(pieces_alive > 22){
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

float evaluate_board(std::array<std::array<int, 8>, 8> board, int pieces_alive, int king_pos[2][2]){
    float score = 0;

    // Get material values
    score += sum_material_values(board, pieces_alive);

    // Get pawn values


    // Get King safety
    if(king_pos[0][0] == -1){ // incase the king is dead
        return score;
    }
    for(int i = -2; i < 2; i++){
        if(i !=0 && king_pos[0][0]+i >= 0 && king_pos[0][0]+i < 8){ 
            for(int j = -2; j < 2; j++){
                if(j !=0 && king_pos[0][1]+j >= 0 && king_pos[0][1]+j < 8){ // Don't want the king itself or positions outside the board
                    if(board[king_pos[0][0]+i][king_pos[0][1]+j] > 0){ // If there's an ally we consider that great
                        score += 2;
                    }
                }
            }
        }else if(i !=0 && king_pos[1][0]+i >= 0 && king_pos[1][0]+i < 8){
            for(int j = -2; j < 2; j++){
                if(j !=0 && king_pos[1][1]+j >= 0 && king_pos[1][1]+j < 8){ // Don't want the king itself or positions outside the board
                    if(board[king_pos[0][0]+i][king_pos[0][1]+j] > 0){ // If there's an ememy we consider that bad
                        score -= 2;
                    }
                }
            }
        }
    }
    

    return score;
}

#endif