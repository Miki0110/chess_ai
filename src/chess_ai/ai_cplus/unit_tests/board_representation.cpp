#include "board_representation.h"

int main() {
    // Initialize the board with a FEN string
    std::string initialFEN = "r1bqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1";
    ChessEngine chessEngine(initialFEN);

    // Display the initial board
    chessEngine.displayBoard();

    // Generate legal moves for white
    bool whiteToMove = true;
    std::vector<Move> moves = chessEngine.generateLegalMoves(whiteToMove);

    // Print generated moves
    for (const Move& move : moves) {
        std::cout << "From: " << move.fromSquare << " To: " << move.toSquare
                  << " Piece: " << move.pieceType << " Captured: " << move.capturedPieceType << std::endl;
    }

    return 0;
}