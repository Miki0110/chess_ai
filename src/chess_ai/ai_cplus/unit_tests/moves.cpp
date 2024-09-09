#include <iostream>
#include <cassert>
#include "ChessBoard.h"
#include "MoveGenerator.h"


void testPossibleMoves() {
    ChessBoard board("rnbqkb1r/pppppppp/5n2/8/8/5N2/PPPPPPPP/RNBQKB1R w KQkq - 0 1");
    auto moves = MoveGenerator::generateLegalMoves(board, true);
    std::cout << "Number of legal moves: " << moves.size() << std::endl;
    for (const auto& move : moves) {
        std::cout << convertMoveToNotation(move) << std::endl;
    }
    board.displayBoard();
}




int main() {
    testPossibleMoves();
    return 0;
}
