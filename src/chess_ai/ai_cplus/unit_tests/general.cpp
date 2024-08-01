#include <iostream>
#include <vector>
#include <chrono>
#include <eval_values.h>
#include <eval_functions.h>
#include <board_representation.h>
#include <search_algorithm.h>
#include <cassert>

// Function to print the board state
void print_board(const std::array<std::array<int, 8>, 8>& board) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            std::cout << board[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Function to run tests
void run_tests() {
    // Test FEN parsing
    Board board1("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    assert(board1.board_to_fen(1) == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // Test move generation
    Board board2("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    auto moves = board2.get_allmoves(1);
    assert(moves.size() == 20);

    // Test evaluation function
    std::array<std::array<int, 8>, 8> test_board = {{
        {2, 3, 4, 6, 5, 4, 3, 2},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {-2, -3, -4, -6, -5, -4, -3, -2}
    }};
    assert(sum_materialvalues(test_board) == 0);

    std::cout << "All tests passed!" << std::endl;
}

// Function to benchmark the MiniMax algorithm
void benchmark_minimax() {
    Board board("rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1");
    
    auto start_time = std::chrono::high_resolution_clock::now();
    MiniMaxResult result = start_minimax(4, &board, true);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << "MiniMax took " << duration.count() << " seconds." << std::endl;
    std::cout << "Best move: " << result.move[0] << "," << result.move[1] << "," << result.move[2] << "," << result.move[3] << std::endl;
    std::cout << "Score: " << result.score << std::endl;
}

int main() {
    run_tests();
    benchmark_minimax();
    return 0;
}
