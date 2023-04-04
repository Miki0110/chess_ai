#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <array>

#include <eval_values.h>
#include <eval_functions.h>
#include <board_representation.h>
#include <search_algorithm.h>


// This program should sit and wait for FEN strings from the python program
int main() {
    std::string input_string;
    std::array<std::array<int, 8>, 8> board = {{
                                               {{-2,-3,-4,-5,-6,-4,-3,-2}},
                                               {{-1,-1,-1,-1,-1,-1,-1,-1}},
                                               {{0,0,0,0,0,0,0,0}},
                                               {{0,0,0,0,0,0,0,0}},
                                               {{0,0,0,0,0,0,0,0}},
                                               {{0,0,0,0,0,0,0,0}},
                                               {{1,1,1,1,1,1,1,1}},
                                               {{2,3,4,5,6,4,3,2}}
                                               }};
    int p_sum = 0;
    int n_sum = 0;
    int board_value = 0;

    std::cout << "Postive value board:" << std::endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board_value = board[i][j];
            if (board_value == 0){
                std::cout << 0 << " ";
            }else{
                std::cout << board_value*mg_value_tables.at(board_value)[i][j] << " ";
                p_sum += board_value*mg_value_tables.at(board_value)[i][j];
            }
        }
    }
    std::cout << "Total value: " << p_sum << std::endl;   
    

    while (std::getline(std::cin, input_string))
    {
        //std::cout << "Recieved string: " << input_string << std::endl;
        // Check if the input string is the termination string
        if (input_string == "close program")
        {
            break;
        }
        // start the board up
        Board board(input_string);
        board.print_board();
        std::vector<std::array<int, 4>> moves = board.get_allmoves(board.current_player);
        MiniMaxResult result;
        result = minimax(4, board, -1000000, 1000000, true);
        std::cout << "Best move: " << result.move[0] << " " << result.move[1] << " " << result.move[2] << " " << result.move[3] << std::endl;
        std::cout << "Score: " << result.score << std::endl;
    }

    std::cout << "C++ program finished" << std::endl;
    return 0;
}

