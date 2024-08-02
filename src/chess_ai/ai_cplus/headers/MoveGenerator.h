#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "ChessBoard.h"
#include "Move.h"
#include <vector>

class MoveGenerator {
public:
    static std::vector<Move> generateLegalMoves(const ChessBoard& board, bool whiteToMove) {
        std::vector<Move> moves;

        uint64_t occupancy = whiteToMove ? board.getWhiteOccupancy() : board.getBlackOccupancy();
        uint64_t enemyOccupancy = whiteToMove ? board.getBlackOccupancy() : board.getWhiteOccupancy();

        for (int pieceType = whiteToMove ? WHITE_PAWN : BLACK_PAWN; pieceType <= (whiteToMove ? WHITE_KING : BLACK_KING); ++pieceType) {
            uint64_t pieceBitboard = board.getPieceBitboard(pieceType);
            while (pieceBitboard) {
                int fromSquare = __builtin_ctzll(pieceBitboard); // Get least significant bit index
                generatePieceMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board);
                pieceBitboard &= pieceBitboard - 1; // Clear least significant bit
            }
        }

        return moves;
    }

private:
    static void generatePieceMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                   uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board) {
        if (pieceType == WHITE_PAWN || pieceType == BLACK_PAWN) {
            generatePawnMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board);
        }
        // Add logic for other pieces...
    }

    static void generatePawnMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                  uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board) {
        int direction = (pieceType == WHITE_PAWN) ? 8 : -8; // Move direction for white and black pawns

        int toSquare = fromSquare + direction;

        if (!(occupancy & (1ULL << toSquare)) && toSquare >= 0 && toSquare < 64) { // Check if the square is empty
            moves.emplace_back(fromSquare, toSquare, pieceType, -1); // Add normal move

            // Check for double move from starting rank
            if ((pieceType == WHITE_PAWN && fromSquare >= 8 && fromSquare <= 15) ||
                (pieceType == BLACK_PAWN && fromSquare >= 48 && fromSquare <= 55)) {
                int doubleToSquare = toSquare + direction;
                if (!(occupancy & (1ULL << doubleToSquare))) {
                    moves.emplace_back(fromSquare, doubleToSquare, pieceType, -1);
                }
            }
        }

        // Capture moves
        int captureDirections[2] = {-1, 1};
        for (int captureDir : captureDirections) {
            int captureSquare = fromSquare + direction + captureDir;
            if (captureSquare >= 0 && captureSquare < 64 && (enemyOccupancy & (1ULL << captureSquare))) {
                int capturedPieceType = -1;
                for (int i = 0; i < 12; ++i) {
                    if (board.getPieceBitboard(i) & (1ULL << captureSquare)) {
                        capturedPieceType = i;
                        break;
                    }
                }
                moves.emplace_back(fromSquare, captureSquare, pieceType, capturedPieceType);
            }
        }
    }
};

#endif