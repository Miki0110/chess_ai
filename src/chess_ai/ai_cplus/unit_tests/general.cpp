#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include "ChessBoard.h"
#include "MoveGenerator.h"


void testMoveApplicationAndUndo() {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; // Starting position
    ChessBoard board(fen);

    // Generate moves for white
    auto moves = MoveGenerator::generateLegalMoves(board, true);
    assert(!moves.empty());

    // Apply the first move
    Move firstMove = moves[0];
    board.applyMove(firstMove);

    // Undo the move
    board.undoMove(firstMove);

    // Check if board is back to initial state
    uint64_t initialOccupancy = 0xFFFF00000000FFFF;
    assert(board.getAllOccupancy() == initialOccupancy);

    std::cout << "Move Application and Undo Test Passed" << std::endl;
}

void testZobristHashing() {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; // Starting position
    ChessBoard board(fen);

    uint64_t initialHash = board.hash();

    // Generate moves and apply one
    auto moves = MoveGenerator::generateLegalMoves(board, true);
    assert(!moves.empty());
    Move firstMove = moves[0];
    board.applyMove(firstMove);

    // Hash should change after move
    uint64_t afterMoveHash = board.hash();
    assert(initialHash != afterMoveHash);

    // Undo move
    board.undoMove(firstMove);

    // Hash should be back to initial state
    uint64_t afterUndoHash = board.hash();
    assert(initialHash == afterUndoHash);

    std::cout << "Zobrist Hashing Test Passed" << std::endl;
}



int main() {
    testMoveApplicationAndUndo();
    testZobristHashing();
    return 0;
}
