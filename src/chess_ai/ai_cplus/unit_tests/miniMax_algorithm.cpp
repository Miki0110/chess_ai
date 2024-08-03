#include <iostream>
#include <cassert>
#include <chrono>
#include "MiniMax.h"
#include "ChessBoard.h"
#include "Move.h"

void test_minimax_correctness() {
    ChessBoard board("rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1");

    Move result = Minimax::findBestMove(board, 9, true);
    std::cout << "Best move: " << convertMoveToNotation(result) << std::endl; // Best move is d2-d4
    assert(result.fromSquare == 12 && result.toSquare == 28); // Best move is d2-d4


    ChessBoard board1("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Move result1 = Minimax::findBestMove(board1, 9, true);
    std::cout << "Best move: " << convertMoveToNotation(result1) << std::endl; // Best move is e2-e4
    assert(result1.fromSquare == 12 && result1.toSquare == 28); // Best move is e2-e4


    ChessBoard board2("r1b1kb1r/3npppp/p1p5/2N3B1/4P1n1/8/PPP2PPP/R3K1NR w KQkq - 0 1");
    Move result2 = Minimax::findBestMove(board2, 9, true);
    std::cout << "Best move: " << convertMoveToNotation(result2) << std::endl; // Best move is c5-d7
    assert(result2.fromSquare == 18 && result2.toSquare == 27); // Best move is c5-d7

    ChessBoard board3("r1b1kb1r/3npppp/p1p5/2N3B1/4P1n1/8/PPP2PPP/R3K1NR w KQkq - 0 1");
    Move result3 = Minimax::findBestMove(board3, 9, true);
     std::cout << "Best move: " << convertMoveToNotation(result3) << std::endl; // Best move is c4-d4
    assert(result3.fromSquare == 20 && result3.toSquare == 28); // Best move is c4-d4
    

    ChessBoard board4("r1b1kb1r/3npppp/p1p5/2N3B1/4P1n1/8/PPP2PPP/R3K1NR w KQkq - 0 1");
    Move result4 =Minimax::findBestMove(board4, 9, true);
    std::cout << "Best move: " << convertMoveToNotation(result4) << std::endl; // Best move is e2-e4
    assert(result4.fromSquare == 12 && result4.toSquare == 28); // Best move is e2-e4

    std::cout << "MiniMax Correctness Test Passed!\n";
}

void test_minimax_time() {
    ChessBoard board("rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1");
    auto start_time = std::chrono::high_resolution_clock::now();
    Move result = Minimax::findBestMove(board, 9, true);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << "MiniMax took " << std::chrono::duration_cast<std::chrono::seconds>(duration).count() << " seconds\n";
    assert(std::chrono::duration_cast<std::chrono::seconds>(duration).count() < 30); // MiniMax should take less than 30 seconds
}

int main() {
    test_minimax_time();
    test_minimax_correctness();
    std::cout << "All MiniMax Algorithm Tests Passed!\n";
    return 0;
}
