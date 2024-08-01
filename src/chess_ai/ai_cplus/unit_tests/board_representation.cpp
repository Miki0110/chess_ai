#include <iostream>
#include <cassert>
#include "board_representation.h"

void test_fen_parsing() {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    std::string fen = board.board_to_fen(1);
    assert(fen == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    std::cout << "FEN Parsing Test Passed!\n";
}

void test_move_generation() {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    auto moves = board.get_allmoves(1);
    assert(moves.size() == 20); // 16 pawns + 4 knights = 20 moves
    std::cout << "Move Generation Test Passed!\n";
}

void test_undo_move() {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.move_piece(1, 0, 3, 0); // Move pawn from a2 to a4
    board.undo_move();
    std::string fen = board.board_to_fen(1);
    assert(fen == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    std::cout << "Undo Move Test Passed!\n";
}

int main() {
    test_fen_parsing();
    test_move_generation();
    test_undo_move();
    std::cout << "All Board Representation Tests Passed!\n";
    return 0;
}
