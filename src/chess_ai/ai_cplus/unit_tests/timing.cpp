#include <iostream>
#include <cassert>
#include <chrono>
#include "ChessBoard.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "Evaluation.h"

void test_move_generation_time() {
    ChessBoard board("rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1");
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++) {
        auto moves = MoveGenerator::generateLegalMoves(board, true);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << "Move Generation took " << duration.count() << " milliseconds\n";
    assert(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() < 100); // Move generation should take less than 100 milliseconds
}

void test_evaluation_time() {
    ChessBoard board("rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1");
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++) {
        int score = Evaluation::evaluateBoard(board);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << "Evaluation took " << duration.count() << " milliseconds\n";
    assert(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() < 100); // Evaluation should take less than 100 milliseconds

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++) {
        int score = Evaluation::evaluateMaterial(board);
    }
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::cout << "Material Evaluation took " << duration.count() << " milliseconds\n";
    assert(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() < 100); // Material evaluation should take less than 100 milliseconds

    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++) {
        int score = Evaluation::evaluatePiecePositions(board);
    }
    end_time = std::chrono::high_resolution_clock::now();
    duration = end_time - start_time;
    std::cout << "Positional Evaluation took " << duration.count() << " milliseconds\n";
}


int main() {
    test_move_generation_time();
    test_evaluation_time();
    std::cout << "All Timing Tests Passed!\n";
    return 0;
}