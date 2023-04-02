#include <iostream>
#include <string>
#include <cstdlib>
#include <evaluation_values.h>

class Board
{
private:
    // Class values
    int en_passent[2] = {-1, -1}; // -1 = no possible passants
    bool black_castle[2] = {false, false}; // Queens-, Kings -side
    bool white_castle[2] = {false, false}; // Queens-, Kings -side
    int current_player = 1; // 1 = white, -1 = black

    std::array<std::array<int, 8>, 8> board;

    void reset_board(){
        board.fill({0, 0, 0, 0, 0, 0, 0, 0});
    }

    void set_board(std::string FEN){
        // Fill the board with zeroes
        reset_board();

        
        int row = 0;
        int col = 0;
        // loop through the FEN string
        for(int i=0; i < FEN.find(" "); i++){
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
    }


public:

    void print_board(){
        // Print the array
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                std::cout << board[i][j] << " ";
            }
        std::cout << std::endl;
        }
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

