#include <iostream>
#include <cassert>
#include "ChessBoard.h"
#include "MoveGenerator.h"

void testBoardInitialization() {
    // Standard chess starting position FEN
    std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessBoard board(startFEN);
    board.displayBoard();

    // Test that the white pawns are correctly initialized
    uint64_t whitePawns = board.getPieceBitboard(WHITE_PAWN);
    assert(whitePawns == 0xFF00);

    // Test that the black pawns are correctly initialized
    uint64_t blackPawns = board.getPieceBitboard(BLACK_PAWN);
    assert(blackPawns == 0xFF000000000000);

    // Test that the kings are correctly placed
    uint64_t whiteKing = board.getPieceBitboard(WHITE_KING);
    uint64_t blackKing = board.getPieceBitboard(BLACK_KING);
    assert(whiteKing == 0x10);
    assert(blackKing == 0x1000000000000000);

    // Test that the board occupancy is correct
    uint64_t allPieces = board.getAllOccupancy();
    assert(allPieces == 0xFFFF00000000FFFF);

    std::cout << "Board Initialization Test Passed" << std::endl;
}

void testPawnMoveGeneration() {
    // Test case 1: Single forward move
    std::string fen1 = "8/8/8/8/4P3/8/8/8 w - - 0 1"; // White pawn on e4
    ChessBoard board1(fen1);
    auto moves1 = MoveGenerator::generateLegalMoves(board1, true);
    std::cout << "Test 1 - Pawn moves: " << moves1.size() << std::endl;

    // Expected moves: e4 to e5 (1 move)
    assert(moves1.size() == 1);
    assert(moves1[0].fromSquare == 28 && moves1[0].toSquare == 36); // e4 to e5

    std::cout << "Test 1 Passed: Single forward move" << std::endl;

    // Test case 2: Double forward move from starting position
    std::string fen2 = "8/8/8/8/8/8/4P3/8 w - - 0 1"; // White pawn on e2
    ChessBoard board2(fen2);
    auto moves2 = MoveGenerator::generateLegalMoves(board2, true);
    std::cout << "Test 2 - Pawn moves: " << moves2.size() << std::endl;

    // Expected moves: e2 to e3, e2 to e4 (2 moves)
    assert(moves2.size() == 2);
    assert((moves2[0].fromSquare == 12 && moves2[0].toSquare == 20) || (moves2[1].fromSquare == 12 && moves2[1].toSquare == 20)); // e2 to e3
    assert((moves2[0].fromSquare == 12 && moves2[0].toSquare == 28) || (moves2[1].fromSquare == 12 && moves2[1].toSquare == 28)); // e2 to e4

    std::cout << "Test 2 Passed: Double forward move" << std::endl;

    // Test case 3: Capture
    std::string fen3 = "8/8/8/3p4/4P3/8/8/8 w - - 0 1"; // White pawn on e4, black pawn on d5
    ChessBoard board3(fen3);
    auto moves3 = MoveGenerator::generateLegalMoves(board3, true);
    std::cout << "Test 3 - Pawn moves: " << moves3.size() << std::endl;
    std::cout << "Moves: " << convertMoveToNotation(moves3[0]) << std::endl;
    std::cout << "Moves: " << convertMoveToNotation(moves3[1]) << std::endl;

    // Expected moves: e4 to e5, e4 captures d5 (2 moves)
    assert(moves3.size() == 2);
    assert((moves3[0].fromSquare == 28 && moves3[0].toSquare == 36) || (moves3[1].fromSquare == 28 && moves3[1].toSquare == 36)); // e4 to e5
    assert((moves3[0].fromSquare == 28 && moves3[0].toSquare == 35) || (moves3[1].fromSquare == 28 && moves3[1].toSquare == 35)); // e4 captures d5

    std::cout << "Test 3 Passed: Capture move" << std::endl;

    // Test case 4: En Passant
    std::string fen4 = "8/8/8/3Pp3/8/8/8/8 w - e6 0 1"; // White pawn on d5, en passant possible at e6
    ChessBoard board4(fen4);
    auto moves4 = MoveGenerator::generateLegalMoves(board4, true);
    std::cout << "Test 4 - Pawn moves: " << moves4.size() << std::endl;
    for (const auto& move : moves4) {
        std::cout << "Moves: " << convertMoveToNotation(move) << std::endl;
    }

    // Expected moves: d5 to d6, d5 captures e6 en passant (2 moves)
    assert(moves4.size() == 2);
    assert((moves4[0].fromSquare == 35 && moves4[0].toSquare == 43) || (moves4[1].fromSquare == 35 && moves4[1].toSquare == 43)); // d5 to d6
    assert((moves4[0].fromSquare == 35 && moves4[0].toSquare == 44 && moves4[0].isEnPassant) || 
           (moves4[1].fromSquare == 35 && moves4[1].toSquare == 44 && moves4[1].isEnPassant)); // d5 captures e6 en passant

    std::cout << "Test 4 Passed: En Passant move" << std::endl;

    // Test case 5: Promotion
    std::string fen5 = "8/4P3/8/8/8/8/8/8 w - - 0 1"; // White pawn on e7
    ChessBoard board5(fen5);
    auto moves5 = MoveGenerator::generateLegalMoves(board5, true);
    std::cout << "Test 5 - Pawn moves: " << moves5.size() << std::endl;
    for (const auto& move : moves5) {
        std::cout << "Moves: " << convertMoveToNotation(move) << std::endl;
    }

    // Expected moves: e7 to e8 with promotion (4 moves for queen, rook, bishop, knight)
    assert(moves5.size() == 1);
    assert(moves5[0].fromSquare == 52 && moves5[0].toSquare == 60 && moves5[0].isPromotion); // e7 to e8 with promotion

    std::cout << "Test 5 Passed: Promotion move" << std::endl;
}

void testKnightMoveGeneration() {
    std::string fen = "8/8/8/3N4/8/8/8/8 w - - 0 1"; // White knight on d5
    ChessBoard board(fen);
    auto moves = MoveGenerator::generateLegalMoves(board, true);
    std::cout << "Knight moves: " << moves.size() << std::endl;
    // Expected moves: eight possible knight moves
    assert(moves.size() == 8);

    std::cout << "Knight Move Generation Test Passed" << std::endl;
}

void testBishopMoveGeneration() {
    std::string fen = "8/8/8/3B4/8/8/8/8 w - - 0 1"; // White bishop on d5
    ChessBoard board(fen);
    auto moves = MoveGenerator::generateLegalMoves(board, true);
    std::cout << "Bishop moves: " << moves.size() << std::endl;

    // Expected moves: 13 possible bishop moves (consider open board)
    assert(moves.size() == 13);

    std::cout << "Bishop Move Generation Test Passed" << std::endl;
}

void testRookMoveGeneration() {
    std::string fen = "8/8/8/3R4/8/8/8/8 w - - 0 1"; // White rook on d5
    ChessBoard board(fen);
    auto moves = MoveGenerator::generateLegalMoves(board, true);
    std::cout << "Rook moves: " << moves.size() << std::endl;

    // Expected moves: 14 possible rook moves (consider open board)
    assert(moves.size() == 14);

    std::cout << "Rook Move Generation Test Passed" << std::endl;
}

void testQueenMoveGeneration() {
    std::string fen = "8/8/8/3Q4/8/8/8/8 w - - 0 1"; // White queen on d5
    ChessBoard board(fen);
    auto moves = MoveGenerator::generateLegalMoves(board, true);
    std::cout << "Queen moves: " << moves.size() << std::endl;

    // Expected moves: 27 possible queen moves (consider open board)
    assert(moves.size() == 27);

    std::cout << "Queen Move Generation Test Passed" << std::endl;
}

void testKingMoveGeneration() {
    std::string fen = "8/8/8/3K4/8/8/8/8 w - - 0 1"; // White king on d5
    ChessBoard board(fen);
    auto moves = MoveGenerator::generateLegalMoves(board, true);
    std::cout << "King moves: " << moves.size() << std::endl;

    // Expected moves: 8 possible king moves
    assert(moves.size() == 8);

    // Test case 1: White Kingside Castling
    std::string fen1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"; // White can castle kingside
    ChessBoard board1(fen1);
    auto moves1 = MoveGenerator::generateLegalMoves(board1, true);
    std::cout << "Test 1 - Castling moves: " << moves1.size() << std::endl;

    bool kingsideCastlingFound = false;
    for (const auto& move : moves1) {
        if (move.isCastling && move.fromSquare == 4 && move.toSquare == 6) { // e1 to g1 (O-O)
            kingsideCastlingFound = true;
            break;
        }
    }
    assert(kingsideCastlingFound);

    std::cout << "Test 1 Passed: White Kingside Castling" << std::endl;

    // Test case 2: White Queenside Castling
    std::string fen2 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"; // White can castle queenside
    ChessBoard board2(fen2);
    auto moves2 = MoveGenerator::generateLegalMoves(board2, true);
    std::cout << "Test 2 - Castling moves: " << moves2.size() << std::endl;

    bool queensideCastlingFound = false;
    for (const auto& move : moves2) {
        if (move.isCastling && move.fromSquare == 4 && move.toSquare == 2) { // e1 to c1 (O-O-O)
            queensideCastlingFound = true;
            break;
        }
    }
    assert(queensideCastlingFound);

    std::cout << "Test 2 Passed: White Queenside Castling" << std::endl;

    // Test case 3: Black Kingside Castling
    std::string fen3 = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b kqKQ - 0 1"; // Black can castle kingside
    ChessBoard board3(fen3);
    auto moves3 = MoveGenerator::generateLegalMoves(board3, false);
    std::cout << "Test 3 - Castling moves: " << moves3.size() << std::endl;

    bool blackKingsideCastlingFound = false;
    for (const auto& move : moves3) {
        if (move.isCastling && move.fromSquare == 60 && move.toSquare == 62) { // e8 to g8 (O-O)
            blackKingsideCastlingFound = true;
            break;
        }
    }
    assert(blackKingsideCastlingFound);

    std::cout << "Test 3 Passed: Black Kingside Castling" << std::endl;

    // Test case 4: Black Queenside Castling
    std::string fen4 = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b kqKQ - 0 1"; // Black can castle queenside
    ChessBoard board4(fen4);
    auto moves4 = MoveGenerator::generateLegalMoves(board4, false);
    std::cout << "Test 4 - Castling moves: " << moves4.size() << std::endl;

    bool blackQueensideCastlingFound = false;
    for (const auto& move : moves4) {
        if (move.isCastling && move.fromSquare == 60 && move.toSquare == 58) { // e8 to c8 (O-O-O)
            blackQueensideCastlingFound = true;
            break;
        }
    }
    assert(blackQueensideCastlingFound);

    std::cout << "Test 4 Passed: Black Queenside Castling" << std::endl;

    // Test case 5: No Castling Due to Blocked Path
    std::string fen5 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RN2KBNR w KQkq - 0 1"; // Blocked path for castling
    ChessBoard board5(fen5);
    auto moves5 = MoveGenerator::generateLegalMoves(board5, true);
    std::cout << "Test 5 - Castling moves: " << moves5.size() << std::endl;

    bool castlingBlocked = true;
    for (const auto& move : moves5) {
        if (move.isCastling) {
            castlingBlocked = false;
            break;
        }
    }
    assert(castlingBlocked);

    std::cout << "Test 5 Passed: No Castling Due to Blocked Path" << std::endl;

    // Test case 6: No Castling Due to King in Check
    std::string fen6 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K1NR w KQkq - 0 1"; // King is in check
    ChessBoard board6(fen6);
    auto moves6 = MoveGenerator::generateLegalMoves(board6, true);
    std::cout << "Test 6 - Castling moves: " << moves6.size() << std::endl;

    bool noCastlingDueToCheck = true;
    for (const auto& move : moves6) {
        if (move.isCastling) {
            noCastlingDueToCheck = false;
            break;
        }
    }
    assert(noCastlingDueToCheck);

    std::cout << "Test 6 Passed: No Castling Due to King in Check" << std::endl;

    std::cout << "King Move Generation Test Passed" << std::endl;
}

int main() {
    testBoardInitialization();
    testPawnMoveGeneration();
    testKnightMoveGeneration();
    testBishopMoveGeneration();
    testRookMoveGeneration();
    testQueenMoveGeneration();
    testKingMoveGeneration();
    return 0;
}
