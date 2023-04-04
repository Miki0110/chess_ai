#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <eval_values.h>
#include <eval_functions.h>
#include <cmath>
#include <vector>
#include <array>

class Board
{
private:
    // Class values
    int en_passant[2] = {-1, -1}; // -1 = no possible passants
    bool black_castle[2] = {false, false}; // Queens-, Kings -side
    bool white_castle[2] = {false, false}; // Queens-, Kings -side

    int king_pos[2][2] = {{7, 4}, {0, 4}}; // [0] = white, [1] = black


    std::array<std::array<int, 8>, 8> board;

    // Struct for moves
    struct ChessMove {
        int from_row;
        int from_col;
        int to_row;
        int to_col;
        int captured_piece;
        int old_passant[2];
        bool old_castle[2];
    };
    // Move history
    std::vector<ChessMove> move_history;

    // Function that resets the board
    void reset_board(){
        board.fill({0, 0, 0, 0, 0, 0, 0, 0});
    }

    // Funtion that sets the board, given a FEN string
    void set_board(std::string FEN){
        // Fill the board with zeroes
        reset_board();

        // Initiate loop values
        int row = 0;
        int col = 0;
        int space_pos = FEN.find(" ");
        // loop through the FEN string
        for(int i=0; i < space_pos; i++){
            char value = FEN[i];
            // Incase it's a new section
            if(value == '/'){
                col = 0;
                row++;
                continue;
            }

            if (isdigit(value)){
                // if it's a number we update the col value
                col += value - '0' - 1;
            }else{
                board[row][col] = piece_to_number.at(value);
            }
            col++;
        }
        // Then check for current player
        if(FEN[space_pos+1] == 'w'){
            current_player = 1;
        }else{
            current_player = -1;
        }

        // Check for castling rights
        for(int i=space_pos+2; i < FEN.find(" ", space_pos+3); i++){
            char value = FEN[i];
            // If there's a minus there are no castling rights
            switch(value){
                case '-':
                    break;
                case 'K':
                    white_castle[1] = true;
                    break;
                case 'Q':
                    white_castle[0] = true;
                    break;
                case 'k':
                    black_castle[1] = true;
                    break;
                case 'q':
                    black_castle[0] = true;
                    break;
            }
        }
        space_pos = FEN.find(" ", space_pos+3);

        // possible en passant
        if(FEN[space_pos+1] != '-'){ // In case there are no passants
            en_passant[0] = ALPHATOCOLS.at(FEN[space_pos+1]);
            en_passant[1] = FEN[space_pos+2] - '0'; // Convert to an integer
        }
    }

    // Function that checks for enemies at a given position
    bool has_enemy(int p_row, int p_col, int side){
        // get the piece at the position
        int piece = board[p_row][p_col];
        int sign = piece > 0 ? 1 : -1;
        // If the piece is 0, there's no piece there
        if(piece == 0){
            return false;
        // If the piece is the same color as the side, there's no enemy
        }else if (sign == side){
           return false;
        }else{
            return true;
        }
    }

    // Function that checks for allies at a given position
    bool has_ally(int p_row, int p_col, int side){
        // get the piece at the position
        int piece = board[p_row][p_col];
        int sign = piece > 0 ? 1 : -1;
        // If the piece is 0, there's no piece there
        if(piece == 0){
            return false;
        // If the piece is the same color as the side there's an ally
        }else if (sign != side){
           return false;
        }else{
            return true;
        }
    }

    // Function to check possible moves -> Rook, Bishop and Queen
    std::vector<std::array<int, 4>> general_move_calc(int p_row, int p_col, bool move_diagonal, bool move_straight){
        // Get the piece we are checking
        int piece = board[p_row][p_col];
        // Output vector
        std::vector<std::array<int, 4>> moves;

        // Check diagonal moves
        if(move_diagonal){
            // check diagonal from top left to bottom right
            for (int i = 1; p_row - i >= 0 && p_col - i >= 0; i++) {
                int pos = board[p_row - i][p_col - i];
                if (pos == 0) {
                    // save the possible move
                    moves.push_back({p_row, p_col, p_row - i, p_col - i});
                }else if((pos > 0) == (piece > 0)){
                    // If the piece is the same color as the side there's an ally
                    break;
                }else{
                    // If the piece is the not same color as the side there's an enemy
                    moves.push_back({p_row, p_col, p_row - i, p_col - i});
                    break;
                }
            }
            for (int i = 1; p_row + i < 8 && p_col + i < 8; i++) {
                int pos = board[p_row + i][p_col + i];
                if (pos == 0) {
                    // save the possible move
                    moves.push_back({p_row, p_col, p_row + i, p_col + i});
                }else if((pos > 0) == (piece > 0)){
                    // If the piece is the same color as the side there's an ally
                    break;
                }else{
                    // If the piece is the not same color as the side there's an enemy
                    moves.push_back({p_row, p_col, p_row + i, p_col + i});
                    break;
                }
            }

            // check diagonal from top right to bottom left
            for (int i = 1; p_row - i >= 0 && p_col + i < 8; i++) {
                int pos = board[p_row - i][p_col + i];
                if (pos == 0) {
                    // save the possible move
                    moves.push_back({p_row, p_col, p_row - i, p_col + i});
                }else if((pos > 0) == (piece > 0)){
                    // If the piece is the same color as the side there's an ally
                    break;
                }else{
                    // If the piece is the not same color as the side there's an enemy
                    moves.push_back({p_row, p_col, p_row - i, p_col + i});
                    break;
                }
            }
            for (int i = 1; p_row + i < 8 && p_col - i >= 0; i++) {
                int pos = board[p_row + i][p_col - i];
                if (pos == 0) {
                    // save the possible move
                    moves.push_back({p_row, p_col, p_row + i, p_col - i});
                }else if((pos > 0) == (piece > 0)){
                    // If the piece is the same color as the side there's an ally
                    break;
                }else{
                    // If the piece is the not same color as the side there's an enemy
                    moves.push_back({p_row, p_col, p_row + i, p_col - i});
                    break;
                }
            }
        }
        // Check straight moves
        if(move_straight){
            // Check up
            for (int i = 1; p_row + i < 8; i++) {
                int pos = board[p_row + i][p_col];
                if (pos == 0) {
                    // save the possible move
                    moves.push_back({p_row, p_col, p_row + i, p_col});
                }else if((pos > 0) == (piece > 0)){
                    // If the piece is the same color as the side there's an ally
                    break;
                }else{
                    // If the piece is the not same color as the side there's an enemy
                    moves.push_back({p_row, p_col, p_row + i, p_col});
                    break;
                }
            }
            // Check below
            for (int i = 1; p_row - i >= 0; i++) {
                int pos = board[p_row - i][p_col];
                if (pos == 0) {
                    // save the possible move
                    moves.push_back({p_row, p_col, p_row - i, p_col});
                }else if((pos > 0) == (piece > 0)){
                    // If the piece is the same color as the side there's an ally
                    break;
                }else{
                    // If the piece is the not same color as the side there's an enemy
                    moves.push_back({p_row, p_col, p_row - i, p_col});
                    break;
                }
            }

            // check right
            for (int i = 1; p_col + i < 8; i++) {
                int pos = board[p_row][p_col + i];
                if (pos == 0) {
                    // save the possible move
                    moves.push_back({p_row, p_col, p_row, p_col + i});
                }else if((pos > 0) == (piece > 0)){
                    // If the piece is the same color as the side there's an ally
                    break;
                }else{
                    // If the piece is the not same color as the side there's an enemy
                    moves.push_back({p_row, p_col, p_row, p_col + i});
                    break;
                }
            }
            // check left
            for (int i = 1; p_col - i >= 0; i++) {
                int pos = board[p_row][p_col - i];
                if (pos == 0) {
                    // save the possible move
                    moves.push_back({p_row, p_col, p_row, p_col - i});
                }else if((pos > 0) == (piece > 0)){
                    // If the piece is the same color as the side there's an ally
                    break;
                }else{
                    // If the piece is the not same color as the side there's an enemy
                    moves.push_back({p_row, p_col, p_row, p_col - i});
                    break;
                }
            }
        }
       
       return moves;
    }
    // Function to check for pawn moves
    std::vector<std::array<int, 4>> pawn_move_calc(int p_row, int p_col){
        int piece = board[p_row][p_col];
        int direction = -1*piece;
        // output vector
        std::vector<std::array<int, 4>> moves;
        // Check for blocking pieces
        if (p_row + direction < 8 && p_row + direction >= 0) {
            if (board[p_row + direction][p_col] == 0) {
                // If the space is free the piece can move there
                moves.push_back({p_row, p_col, p_row + direction, p_col});
                // if we are at the starting position we can move two spaces
                if ((p_row == 6 && piece == 1) || (p_row == 1 && piece == -1)) {
                    if (board[p_row + direction*2][p_col] == 0) {
                        moves.push_back({p_row, p_col, p_row + direction*2, p_col});
                    }
                }
            }
        }
        // Check for capturing pieces
        if (p_row + direction < 8 && p_row + direction >= 0) {
            if (p_col + 1 < 8) {
                if (has_enemy(p_row + direction, p_col + 1, piece)){
                    // If there is an enemy we can capture it
                    moves.push_back({p_row, p_col, p_row + direction, p_col + 1});
                }
            }
            if (p_col - 1 >= 0) {
                if (has_enemy(p_row + direction, p_col - 1, piece)) {
                    // If there is an enemy we can capture it
                    moves.push_back({p_row, p_col, p_row + direction, p_col - 1});
                }
            }
        }

        // Check for en passant
        if(en_passant[0] != -1){
            // Check if the pawn is in the correct position
            int possible_pos[4] = {en_passant[0]+direction, en_passant[1]-1, en_passant[0]+direction, en_passant[1]+1};
            if ((p_row == possible_pos[0] && p_col == possible_pos[1]) || (p_row == possible_pos[2] && p_col == possible_pos[3])) {
                    // If the pawn is in the correct row and column we can capture it
                    moves.push_back({p_row, p_col, en_passant[0], en_passant[1]});
            }
        }
        return moves;
    }
    // Function to check for knight moves
    std::vector<std::array<int, 4>> knight_move_calc(int p_row, int p_col){
        int piece = board[p_row][p_col];
        int side = piece > 0 ? 1 : -1;

        // output vector
        std::vector<std::array<int, 4>> moves;

        // Check for all possible moves
        int possible_pos[8][2] = {{p_row+2, p_col+1}, {p_row+2, p_col-1}, {p_row-2, p_col+1}, {p_row-2, p_col-1}, {p_row+1, p_col+2}, {p_row+1, p_col-2}, {p_row-1, p_col+2}, {p_row-1, p_col-2}};
        for (int i = 0; i < 8; i++) {
            if (possible_pos[i][0] < 8 && possible_pos[i][0] >= 0 && possible_pos[i][1] < 8 && possible_pos[i][1] >= 0) {
                if (board[possible_pos[i][0]][possible_pos[i][1]] == 0 || has_enemy(possible_pos[i][0], possible_pos[i][1], side)) {
                    // If the space is free or has an enemy we can move there
                    moves.push_back({p_row, p_col, possible_pos[i][0], possible_pos[i][1]});
                }
            }
        }
        return moves;
    }
    // Function to check for King moves
    std::vector<std::array<int, 4>> king_move_calc(int p_row, int p_col){
        int piece = board[p_row][p_col];
        int side = piece > 0 ? 1 : -1;

        // output vector
        std::vector<std::array<int, 4>> moves;

        // Check for all possible moves
        int possible_pos[8][2] = {{p_row+1, p_col}, {p_row-1, p_col}, {p_row, p_col+1}, {p_row, p_col-1}, {p_row+1, p_col+1}, {p_row+1, p_col-1}, {p_row-1, p_col+1}, {p_row-1, p_col-1}};
        for (int i = 0; i < 8; i++) {
            if (possible_pos[i][0] < 8 && possible_pos[i][0] >= 0 && possible_pos[i][1] < 8 && possible_pos[i][1] >= 0) {
                if (board[possible_pos[i][0]][possible_pos[i][1]] == 0 || has_enemy(possible_pos[i][0], possible_pos[i][1], side)) {
                    // If the space is free or has an enemy we can move there
                    moves.push_back({p_row, p_col, possible_pos[i][0], possible_pos[i][1]});
                }
            }
        }
        // Check for castling
        if (side == 1) {
            // Check for white castling and if the king is being blocked
            if (white_castle[0] && board[7][1] == 0 && board[7][2] == 0 && board[7][3] == 0) {
                // Move the position back if possible
                moves.push_back({p_row, p_col, p_row, p_col-2});
            }
            if (white_castle[1] && board[7][5] == 0 && board[7][6] == 0) {
                // Check if the king and rook are being blocked
                moves.push_back({p_row, p_col, p_row, p_col+2});
            }
        }else{
            // Check for white castling and if the king is being blocked
            if (black_castle[0] && board[0][1] == 0 && board[0][2] == 0 && board[0][3] == 0) {
                // Move the position back if possible
                moves.push_back({p_row, p_col, p_row, p_col-2});
            }
            if (black_castle[1] && board[0][5] == 0 && board[0][6] == 0) {
                // Check if the king and rook are being blocked
                moves.push_back({p_row, p_col, p_row, p_col+2});
            }
        }
        return moves;
    }

    // Function to get valid moves for a piece
    std::vector<std::array<int, 4>> get_valid_moves(int p_row, int p_col){
        int piece = board[p_row][p_col];
        std::vector<std::array<int, 4>> moves;

        // Check for the piece type
        switch (std::abs(piece)) {
            case 1:
                // Pawn
                moves = pawn_move_calc(p_row, p_col);
                break;
            case 2:
                // Rook
                moves = general_move_calc(p_row, p_col, 0, 1);
                break;
            case 3:
                // Knight
                moves = knight_move_calc(p_row, p_col);
                break;
            case 4:
                // Bishop
                moves = general_move_calc(p_row, p_col, 1, 0);
                break;
            case 6:
                // Queen
                moves = general_move_calc(p_row, p_col, 1, 1);
                break;
            case 5:
                // King
                moves = king_move_calc(p_row, p_col);
                break;
            default:
                // If the piece is not valid return an empty vector
                return moves;
        }
        // TODO: Check for checkmates
        return moves;
    }

public:
    // Class values
    int current_player = 1; // 1 = white, -1 = black

    // Function to undo a move
    void undo_move(){
        // Get the last move
        ChessMove move = move_history.back();
        // Remove the move from the history
        move_history.pop_back();
        // Get the piece
        int piece = board[move.to_row][move.to_col];
        // Move the piece back
        board[move.from_row][move.from_col] = piece;
        // Remove the piece from the old position
        board[move.to_row][move.to_col] = move.captured_piece;
        // set back passant
        en_passant[0] = move.old_passant[0];
        en_passant[1] = move.old_passant[1];
        // set back castling
        if (piece > 0) {
            white_castle[0] = move.old_castle[0];
            white_castle[1] = move.old_castle[1];
        }else{
            black_castle[0] = move.old_castle[0];
            black_castle[1] = move.old_castle[1];
        }
        // Set back king pos
        if (piece == 5) {
            king_pos[0][0] = move.from_row;
            king_pos[0][1] = move.from_col;
        }else if (piece == -5){
            king_pos[1][0] = move.from_row;
            king_pos[1][1] = move.from_col;
        }
    }

        // Function to move pieces
    void move_piece(int start_row, int start_col, int end_row, int end_col){
        // retrieve the piece
        int piece = board[start_row][start_col];
        int sign = piece > 0 ? 1 : -1;
        // values for saving last move
        bool old_castle[2];
        int old_passant[2] = {en_passant[0], en_passant[1]};
        if(sign == 1){
            old_castle[0] = white_castle[0];
            old_castle[1] = white_castle[1];
        }else{
            old_castle[0] = black_castle[0];
            old_castle[1] = black_castle[1];
        }

        // Check if the piece is a pawn
        if (std::abs(piece) == 1){
            // Check if the pawn is moving two steps and if there are enemies nearby
            if ((std::abs(start_row - end_row) == 2) &&
               (end_col <= 7 && board[end_row][end_col+1] == -1*piece) ||
               (end_col >= 0 && board[end_row][end_col-1] == -1*piece)){
                    // Set the possible move for en passant
                    en_passant[0] = end_row+piece;
                    en_passant[1] = end_col;
                }else{
                    // Else we reset the en passant
                    en_passant[0] = -1;
                    en_passant[1] = -1;
                }
            // Check if we completed an en passant instead
            if (end_row == en_passant[0] && end_col == en_passant[1]){
                // Remove the piece
                board[start_row][end_col] = 0;
            }
        }else{ // Incase we are not pawns we reset the passants
            en_passant[0] = -1;
            en_passant[1] = -1;
        }

        // Check for Rook moves
        if (std::abs(piece) == 2){
            // Check if we are moving the white rook
            if (sign == 1){
                // Check if we are moving the left rook
                if (start_col == 0){
                    white_castle[0] = false;
                }else if(start_col == 7){
                    white_castle[1] = false;
                }
            }else{
                // Check if we are moving the left rook
                if (start_col == 0){
                    black_castle[0] = false;
                }else if(start_col == 7){
                    black_castle[1] = false;
                }
            }
        }

        // Check for King moves
        if (std::abs(piece) == 5){
            // Check if we are moving the white king
            if (sign == 1){
                if (white_castle[0] && end_col == 1){
                    // Move the rook
                    board[7][0] = 0;
                    board[7][2] = 2;
                }else if (white_castle[1] && end_col == 6){
                    // Move the rook
                    board[7][7] = 0;
                    board[7][4] = 2;
                }
                // Set castling to false
                white_castle[0] = false;
                white_castle[1] = false;

                // update the king position
                king_pos[0][0] = end_row;
                king_pos[0][1] = end_col;
            }else{
                if (black_castle[0] && end_col == 1){
                    // Move the rook
                    board[0][0] = 0;
                    board[0][3] = -2;
                }else if (black_castle[1] && end_col == 6){
                    // Move the rook
                    board[0][7] = 0;
                    board[0][5] = -2;
                }
                // Set castling to false
                black_castle[0] = false;
                black_castle[1] = false;

                // update the king position
                king_pos[1][0] = end_row;
                king_pos[1][1] = end_col;
            }
        }
        // Get the piece captued
        int captured_piece = board[end_row][end_col];
        // save the move
        ChessMove move = {start_row, start_col, end_row, end_col, captured_piece, {old_passant[0], old_passant[1]}, {old_castle[0], old_castle[1]}};
        
        move_history.push_back(move);
        // Move the piece
        board[end_row][end_col] = piece;
        // Remove the piece from the old position
        board[start_row][start_col] = 0;
    }

    void print_board(){
        // Print the array
        std::cout << "Current board:" << std::endl;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                std::cout << board[i][j] << " ";
            }
        std::cout << std::endl;
        }

        std::cout << "White castling rights: " << white_castle[0] << white_castle[1] << std::endl;
        std::cout << "Black castling rights: " << black_castle[0] << black_castle[1] << std::endl;
        std::cout << "en passant rights: " << en_passant[0] << en_passant[1] << std::endl;
    }
    
    std::vector<std::array<int, 4>> get_allmoves(int side){
        std::vector<std::array<int, 4>> moves;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (board[i][j] != 0 && board[i][j] > 0 == side > 0) {
                    std::vector<std::array<int, 4>> temp = get_valid_moves(i, j);
                    moves.insert(moves.end(), temp.begin(), temp.end());
                }
            }
        }
        return moves;
    }

    float get_board_value(){
        float score = evaluate_board(board, king_pos);
        return score;
    }

    // Constructer, used for setting the board up
    Board(std::string FEN){
        set_board(FEN);
    }
    //~Board();
};

#endif
