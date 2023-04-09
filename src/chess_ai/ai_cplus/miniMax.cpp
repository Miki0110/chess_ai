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
    // For testing specific moves
    /*std::string test = "rnb1k2r/3p2b1/3P4/p2B1P2/7p/1P2p1P1/PP2P2P/2RK3R b kq - 22 39";
    Board board(test);
    std::vector<std::array<int, 4>> moves = board.debug_moves(0,4);
    for(int i = 0; i < moves.size(); i++){
        std::cout << moves[i][0] << "," << moves[i][1] << "," << moves[i][2] << "," << moves[i][3] << std::endl;
    }*/


    // For testing randomly
    /*std::string test = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Board board(test);
    for(int i = 0; i < 10; i++){
        std::array<std::array<int, 8>, 8> ori_board = board.get_board();
        int move[4] = {rand() % 8, rand() % 8, rand() % 8, rand() % 8};
        board.move_piece(move[0], move[1], move[2], move[3]);
        std::array<std::array<int, 8>, 8> after_board = board.get_board();
        board.undo_move();
        if(!board.areEqual(ori_board)){
            std::cout << "Something went wrong" << std::endl;
            std::cout << "Move: " << move[0] << "," << move[1] << "," << move[2] << "," << move[3] << std::endl;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                std::cout << after_board[i][j] << " ";
            }
        std::cout << std::endl;
        }
            board.print_board();
        }
    }*/

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
            result = minimax(depth, &board, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true);
        }else{
            result = minimax(depth, &board, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
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

