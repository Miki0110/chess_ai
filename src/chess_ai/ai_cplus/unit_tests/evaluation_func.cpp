#include <iostream>
#include <cassert>
#include "Evaluation.h"
#include "ChessBoard.h"

void test_material_evaluation() {
    ChessBoard board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    int score = Evaluation::evaluateBoard(board);
    std::cout << "Material Score: " << score << std::endl;
    assert(score == 0); // Balanced material

    // White side advantage
    ChessBoard board1("rnb1kbnr/pppppppp/8/8/8/8/PPPPPPP1/RNBQKBNQ w KQkq - 0 1");
    int score1 = Evaluation::evaluateBoard(board1);
    std::cout << "Material Score: " << score1 << std::endl;
    assert(score1 > 0); // White has advantage

    // Black side advantage
    ChessBoard board2("rnbqkbnq/ppppppp1/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    int score2 = Evaluation::evaluateBoard(board2);
    std::cout << "Material Score: " << score2 << std::endl;
    assert(score2 < 0); // Black has advantage

    std::cout << "Material Evaluation Test Passed!\n";
}

void test_positional_evaluation() {
    // No side advantage
    ChessBoard board1("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    int score1 = Evaluation::evaluatePiecePositions(board1);
    std::cout << "Positional Score: " << score1 << std::endl;
    //assert(score == 0); // Balanced position

    // White side advantage with Rook activity
    ChessBoard board2("r4rk1/pppq1ppp/2n1b3/3n4/3P4/2NBP3/PPP2PPP/R1BQR1K1 w - - 0 1");
    int score2 = Evaluation::evaluatePiecePositions(board2);
    std::cout << "Positional Score: " << score2 << std::endl;
    //assert(score > 50); // White has advantage

    // White side advantage with Knight activity
    ChessBoard board3("r1bq1rk1/ppp1ppbp/3p1np1/8/3PN3/2N5/PPP2PPP/R1BQ1RK1 w - - 0 1");
    int score3 = Evaluation::evaluatePiecePositions(board3);
    std::cout << "Positional Score: " << score3 << std::endl;
    //assert(score1 > 50); // White has advantage

    // Black side advantage with Rook activity
    ChessBoard board4("2rr2k1/1p3ppp/p2qp3/8/4P3/1QP1B3/P3NPPP/R3R1K1 b - - 0 1");
    int score4 = Evaluation::evaluatePiecePositions(board4);
    std::cout << "Positional Score: " << score4 << std::endl;
    //assert(score2 < -50); // Black has advantage

    // Black side advantage with Knight activity
    ChessBoard board5("r4rk1/ppp1bppp/2n1p3/3qP3/3P2n1/2P1B3/PP1NQPPP/R1B2RK1 b - - 0 1");
    int score5 = Evaluation::evaluatePiecePositions(board5);
    std::cout << "Positional Score: " << score5 << std::endl;
    //assert(score3 < -50); // Black has advantage
}

int main() {
    test_material_evaluation();
    test_positional_evaluation();
    std::cout << "All Evaluation Function Tests Passed!\n";
    return 0;
}
