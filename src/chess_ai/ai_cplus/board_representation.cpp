#include <iostream>
#include <string>
#include <cstdlib>
#include <evaluation_values.h>
#include <cmath>

class Board
{
private:
    // Class values
    int en_passant[2] = {-1, -1}; // -1 = no possible passants
    bool black_castle[2] = {false, false}; // Queens-, Kings -side
    bool white_castle[2] = {false, false}; // Queens-, Kings -side
    int current_player = 1; // 1 = white, -1 = black

    std::array<std::array<int, 8>, 8> board;

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
            en_passant[1] = FEN[space_pos+2] - '0';
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

    // Function to move pieces
    void move_piece(int start_row, int start_col, int end_row, int end_col){
        // Save the moves
            //TODO: Save the moves

        // retrieve the piece
        int piece = board[start_row][start_col];
        int sign = piece > 0 ? 1 : -1;

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
            }else{
                if (black_castle[0] && end_col == 1){
                    // Move the rook
                    board[0][0] = 0;
                    board[0][2] = -2;
                }else if (black_castle[1] && end_col == 6){
                    // Move the rook
                    board[0][7] = 0;
                    board[0][4] = -2;
                }
                // Set castling to false
                black_castle[0] = false;
                black_castle[1] = false;
            }
        }

        // Move the piece
        board[end_row][end_col] = piece;
        // Remove the piece from the old position
        board[start_row][start_col] = 0;
    }

    void undo_move(){
        //TODO: Undo the last move
    }

public:

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

    // Constructer, used for setting the board up
    Board(std::string FEN){
        set_board(FEN);
    }
    //~Board();
};



// This program should sit and wait for FEN strings from the python program
int main() {
   std::string input_string;

    while (std::getline(std::cin, input_string))
    {
        std::cout << "Recieved string: " << input_string << std::endl;
        // Check if the input string is the termination string
        if (input_string == "close program")
        {
            break;
        }
        // start the board up
        Board board(input_string);
        board.print_board();
    }

    std::cout << "C++ program finished" << std::endl;
    return 0;
}

