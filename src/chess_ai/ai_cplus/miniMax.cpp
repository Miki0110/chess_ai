#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <array>
#include <sstream>

#include <eval_values.h>
#include <eval_functions.h>
#include <board_representation.h>
#include <search_algorithm.h>


// This program should sit and wait for FEN strings from the python program
int main() {
    std::string input_string;
    std::string test = "rnbqk2r/1p4bp/p4p2/2pP4/3P4/2P2PP1/PP4BP/R2QK2R b kqKQ - 17 25";
    Board board(test);

    board.print_board();
    std::vector<std::array<int, 4>> moves = board.debug_moves(1, 6);
    for(int i = 0; i < moves.size(); i++){
        std::cout << moves[i][0] << "," << moves[i][1] << "," << moves[i][2] << "," << moves[i][3] << std::endl;
    }

    while (std::getline(std::cin, input_string))
    {
        //std::cout << "Recieved string: " << input_string << std::endl;
        // Check if the input string is the termination string
        if (input_string == "close program")
        {
            break;
        }
        std::cout << "input string: " << input_string << std::endl;
        // Get the string stream
        std::stringstream input_ss(input_string);
        int number;
        std::string fen;
        
        // Extract the number
        input_ss >> number;

        // Extract the comma separator
        char comma;
        input_ss >> comma;

        // Extract the rest of the string
        std::getline(input_ss, fen);

        std::cout << "number: " << number << std::endl;
        std::cout << "fen: " << fen << std::endl;
        // start the board up
        Board board(fen);
        board.print_board();
        std::vector<std::array<int, 4>> moves = board.get_allmoves(number);
        MiniMaxResult result;
        if(number == 1){
            result = minimax(4, &board, -1000000, 1000000, true);
        }else{
            result = minimax(4, &board, -1000000, 1000000, false);
        }
        
        std::cout << "Best move: " << result.move[0] << "," << result.move[1] << "," << result.move[2] << "," << result.move[3] << std::endl;
        std::cout << "Score: " << result.score << std::endl;
        std::cout << "We are done" << std::endl;
    }

    std::cout << "C++ program finished" << std::endl;
    return 0;
}

