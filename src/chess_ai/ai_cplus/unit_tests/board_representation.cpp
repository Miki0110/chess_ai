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

void test_pieces_alive(){
    Board board("r1b1kb1r/3npppp/p1p5/2N3B1/4P1n1/8/PPP2PPP/R3K1NR w KQkq - 0 1");
    int pieces_alive = board.pieces_alive;
    assert(pieces_alive == 26);
    board.move_piece(3, 2, 1, 3);
    pieces_alive = board.pieces_alive;
    assert(pieces_alive == 25);
    board.undo_move();
    pieces_alive = board.pieces_alive;
    assert(pieces_alive == 26);

    Board board1("r3kb1r/3bp1pp/p1P5/8/6q1/8/PPP2PP1/1K4NR b kq - 0 1");
    pieces_alive = board1.pieces_alive;
    assert(pieces_alive == 19);
    board1.move_piece(2, 2, 1, 3);
    pieces_alive = board1.pieces_alive;
    assert(pieces_alive == 18);
    board1.move_piece(0, 4, 1, 3);
    pieces_alive = board1.pieces_alive;
    assert(pieces_alive == 17);
    board1.move_piece(5, 6, 5, 5);
    pieces_alive = board1.pieces_alive;
    assert(pieces_alive == 17);
    board1.undo_move();
    pieces_alive = board1.pieces_alive;
    assert(pieces_alive == 17);
    board1.undo_move();
    pieces_alive = board1.pieces_alive;
    assert(pieces_alive == 18);
    board1.undo_move();
    pieces_alive = board1.pieces_alive;
    assert(pieces_alive == 19);

    std::cout << "Pieces Alive Test Passed!\n";
}

int main() {
    test_pieces_alive();
    test_fen_parsing();
    test_move_generation();
    test_undo_move();
    std::cout << "All Board Representation Tests Passed!\n";
    return 0;
}
