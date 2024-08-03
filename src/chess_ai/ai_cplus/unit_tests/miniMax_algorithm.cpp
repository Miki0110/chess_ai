#include <iostream>
#include <cassert>
#include "search_algorithm.h"
#include "board_representation.h"

void test_minimax_correctness() {
    Board board("rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1");
    MiniMaxResult result = start_minimax(10, &board, true);
    std::cout << "Best move: " << result.move[0] << "," << result.move[1] << "," << result.move[2] << "," << result.move[3] << std::endl;
    assert(result.move[0] == 6 && result.move[1] == 3); // Best move is d2-d4


    Board board1("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    MiniMaxResult result1 = start_minimax(10, &board1, true);
    std::cout << "Best move: " << result1.move[0] << "," << result1.move[1] << "," << result1.move[2] << "," << result1.move[3] << std::endl;
    assert(result1.move[0] == 6 && result1.move[1] == 4); // Best move is e2-e4


    Board board2("r1b1kb1r/3npppp/p1p5/2N3B1/4P1n1/8/PPP2PPP/R3K1NR w KQkq - 0 1");
    MiniMaxResult result2 = start_minimax(10, &board2, true);
    std::cout << "Best move: " << result2.move[0] << "," << result2.move[1] << "," << result2.move[2] << "," << result2.move[3] << std::endl;
    assert(result2.move[0] == 3 && result2.move[1] == 2); // Best move is c5-d7

    Board board3("r1b1kb1r/3npppp/p1p5/2N3B1/4P1n1/8/PPP2PPP/R3K1NR w KQkq - 0 1");
    MiniMaxResult result3 = start_minimax(10, &board3, true);
    std::cout << "Best move: " << result3.move[0] << "," << result3.move[1] << "," << result3.move[2] << "," << result3.move[3] << std::endl;
    assert(result3.move[0] == 3 && result3.move[1] == 2); // Best move is c4-d4

    Board board4("r1b1kb1r/3npppp/p1p5/2N3B1/4P1n1/8/PPP2PPP/R3K1NR w KQkq - 0 1");
    MiniMaxResult result4 = start_minimax(10, &board4, true);
    std::cout << "Best move: " << result4.move[0] << "," << result4.move[1] << "," << result4.move[2] << "," << result4.move[3] << std::endl;
    assert(result4.move[0] == 6 && result4.move[1] == 4); // Best move is e2-e4

    std::cout << "MiniMax Correctness Test Passed!\n";
}

void test_minimax_time() {
    Board board("rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1");
    auto start_time = std::chrono::high_resolution_clock::now();
    MiniMaxResult result = start_minimax(9, &board, true);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << "MiniMax took " << std::chrono::duration_cast<std::chrono::seconds>(duration).count() << " seconds\n";
    assert(std::chrono::duration_cast<std::chrono::seconds>(duration).count() < 30); // MiniMax should take less than 30 seconds
}

int main() {
    //test_minimax_time();
    test_minimax_correctness();
    std::cout << "All MiniMax Algorithm Tests Passed!\n";
    return 0;
}
