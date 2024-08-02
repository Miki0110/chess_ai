#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <array>
#include <sstream>
#include <limits>

#include <eval_values.h>
#include <eval_functions.h>
#include <board_representation.h>
#include <search_algorithm.h>
#include <random>


// This program should sit and wait for FEN strings from the python program
int main() {
    std::string input_string;

    while (std::getline(std::cin, input_string))
    {
        //std::cout << "Recieved string: " << input_string << std::endl;
        // Check if the input string is the termination string
        if (input_string == "close program")
        {
            break;
        }
        // Get the string stream
        std::stringstream input_ss(input_string);
        int player;
        int depth;
        std::string fen;
        
        // Extract the number
        input_ss >> player;

        // Extract comma
        char comma;
        input_ss >> comma;

        // Extract depth
        input_ss >> depth;

        // Extract comma
        input_ss >> comma;

        // Extract the rest of the string
        std::getline(input_ss, fen);

        std::cout << "FEN: " << fen << std::endl;

        // start the board up   
        Board board(fen);
        std::vector<std::array<int, 4>> moves = board.get_allmoves(player);
        MiniMaxResult result;
        if(player == 1){
            result = start_minimax(depth, &board, true);
        }else{
            result = start_minimax(depth, &board, false);
        }
        std::vector<std::array<int, 4>> yup = board.debug_moves(result.move[0],result.move[1]);
        for(int i = 0; i < yup.size(); i++){
        std::cout << yup[i][0] << "," << yup[i][1] << "," << yup[i][2] << "," << yup[i][3] << std::endl;
        }
        
        std::cout << "Best move: " << result.move[0] << "," << result.move[1] << "," << result.move[2] << "," << result.move[3] << std::endl;
        std::cout << "Score: " << result.score << std::endl;
        std::cout << "We are done" << std::endl;
    }

    std::cout << "C++ program finished" << std::endl;
    return 0;
}

