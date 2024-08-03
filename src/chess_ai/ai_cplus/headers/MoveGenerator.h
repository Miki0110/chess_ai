#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "ChessBoard.h"
#include "Move.h"
#include <vector>
#include <iostream>

class MoveGenerator {
public:
    static std::vector<Move> generateLegalMoves(const ChessBoard& board, bool whiteToMove) {
        std::vector<Move> moves;

        uint64_t occupancy = whiteToMove ? board.getWhiteOccupancy() : board.getBlackOccupancy();
        uint64_t enemyOccupancy = whiteToMove ? board.getBlackOccupancy() : board.getWhiteOccupancy();

        for (int pieceType = whiteToMove ? WHITE_PAWN : BLACK_PAWN; 
             pieceType <= (whiteToMove ? WHITE_KING : BLACK_KING); ++pieceType) {
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
        switch (pieceType) {
            case WHITE_PAWN:
            case BLACK_PAWN:
                generatePawnMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board);
                break;
            case WHITE_KNIGHT:
            case BLACK_KNIGHT:
                generateKnightMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board);
                break;
            case WHITE_BISHOP:
            case BLACK_BISHOP:
                generateBishopMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board);
                break;
            case WHITE_ROOK:
            case BLACK_ROOK:
                generateRookMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board);
                break;
            case WHITE_QUEEN:
            case BLACK_QUEEN:
                generateQueenMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board);
                break;
            case WHITE_KING:
            case BLACK_KING:
                generateKingMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board);
                break;
        }
    }

    static void generatePawnMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                  uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board) {
        int direction = (pieceType == WHITE_PAWN) ? 8 : -8; // Move direction for white and black pawns
        int startRank = (pieceType == WHITE_PAWN) ? 1 : 6;
        int promotionRank = (pieceType == WHITE_PAWN) ? 7 : 0;

        int toSquare = fromSquare + direction;

        // Single square move
        if (!(occupancy & (1ULL << toSquare)) && toSquare >= 0 && toSquare < 64) { // Check if the square is empty
            if (toSquare / 8 == promotionRank) { // Check for promotion
                moves.emplace_back(fromSquare, toSquare, pieceType, -1, true); // Promotion to queen
            } else {
                moves.emplace_back(fromSquare, toSquare, pieceType, -1); // Add normal move

                // Check for double move from starting rank
                if ((fromSquare / 8) == startRank) {
                    int doubleToSquare = toSquare + direction;
                    if (!(occupancy & (1ULL << doubleToSquare))) {
                        moves.emplace_back(fromSquare, doubleToSquare, pieceType, -1);
                    }
                }
            }
        }

        // Capture moves
        int captureDirections[2] = {-1, 1};
        for (int captureDir : captureDirections) {
            int captureSquare = fromSquare + direction + captureDir;
            if (captureSquare >= 0 && captureSquare < 64 && (enemyOccupancy & (1ULL << captureSquare))) {
                int capturedPieceType = board.getPieceOnSquare(captureSquare);
                if (captureSquare / 8 == promotionRank) { // Check for promotion
                    moves.emplace_back(fromSquare, captureSquare, pieceType, capturedPieceType, true); // Promotion to queen
                } else {
                    moves.emplace_back(fromSquare, captureSquare, pieceType, capturedPieceType);
                }
            }
        }

        // En passant captures
        int enPassantSquare = board.getEnPassantSquare();
        if (enPassantSquare != -1) {
            for (int captureDir : captureDirections) {
                int enPassantCaptureSquare = fromSquare + direction + captureDir;
                if (enPassantCaptureSquare == enPassantSquare) {
                    moves.emplace_back(fromSquare, enPassantSquare, pieceType, (pieceType == WHITE_PAWN) ? BLACK_PAWN : WHITE_PAWN, false, true);
                }
            }
        }
    }

    static void generateKnightMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                    uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board) {
        // Define all possible knight move offsets
        static const int knightOffsets[8] = {-17, -15, -10, -6, 6, 10, 15, 17};
        for (int offset : knightOffsets) {
            int toSquare = fromSquare + offset;
            if (toSquare >= 0 && toSquare < 64 && !((occupancy & (1ULL << toSquare)))) {
                int capturedPieceType = -1;
                if (enemyOccupancy & (1ULL << toSquare)) {
                    capturedPieceType = board.getPieceOnSquare(toSquare);
                }
                moves.emplace_back(fromSquare, toSquare, pieceType, capturedPieceType);
            }
        }
    }

    static void generateBishopMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                    uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board) {
        generateSlidingPieceMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board, true, false);
    }

    static void generateRookMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                  uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board) {
        generateSlidingPieceMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board, false, true);
    }

    static void generateQueenMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                   uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board) {
        generateSlidingPieceMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board, true, true);
    }

    static void generateKingMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                  uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board) {
        // Define all possible king move offsets
        static const int kingOffsets[8] = {-9, -8, -7, -1, 1, 7, 8, 9};
        for (int offset : kingOffsets) {
            int toSquare = fromSquare + offset;
            if (toSquare >= 0 && toSquare < 64 && !((occupancy & (1ULL << toSquare)))) {
                int capturedPieceType = -1;
                if (enemyOccupancy & (1ULL << toSquare)) {
                    capturedPieceType = board.getPieceOnSquare(toSquare);
                }
                moves.emplace_back(fromSquare, toSquare, pieceType, capturedPieceType);
            }
        }

        // Castling moves
        if (pieceType == WHITE_KING || pieceType == BLACK_KING) {
            generateCastlingMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy, board);
        }
    }

    static void generateCastlingMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                      uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board) {
        // Check castling rights and if squares between are empty and not under attack
        if (board.canCastleKingside(pieceType)) {
            if (!(occupancy & (1ULL << (fromSquare + 1))) && !(occupancy & (1ULL << (fromSquare + 2)))) {
                moves.emplace_back(fromSquare, fromSquare + 2, pieceType, -1, false, false, true);
            }
        }

        if (board.canCastleQueenside(pieceType)) {
            if (!(occupancy & (1ULL << (fromSquare - 1))) && !(occupancy & (1ULL << (fromSquare - 2))) && !(occupancy & (1ULL << (fromSquare - 3)))) {
                moves.emplace_back(fromSquare, fromSquare - 2, pieceType, -1, false, false, true);
            }
        }
    }

    static void generateSlidingPieceMoves(int fromSquare, int pieceType, std::vector<Move>& moves,
                                      uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board,
                                      bool diagonal, bool straight) {
    // Straight line moves (rank and file)
    static const int straightOffsets[4] = {-8, -1, 1, 8};
    // Diagonal moves (for bishop and queen)
    static const int diagonalOffsets[4] = {-9, -7, 7, 9};

    if (straight) {
        for (int offset : straightOffsets) {
            generateSlidingMovesInDirection(fromSquare, offset, pieceType, moves, occupancy, enemyOccupancy, board, false);
        }
    }

    if (diagonal) {
        for (int offset : diagonalOffsets) {
            generateSlidingMovesInDirection(fromSquare, offset, pieceType, moves, occupancy, enemyOccupancy, board, true);
        }
    }
}

static void generateSlidingMovesInDirection(int fromSquare, int offset, int pieceType, std::vector<Move>& moves,
                                            uint64_t occupancy, uint64_t enemyOccupancy, const ChessBoard& board, bool isDiagonal) {
        int toSquare = fromSquare + offset;

        while (toSquare >= 0 && toSquare < 64) {
            // Calculate the current rank and file
            int fromRank = fromSquare / 8;
            int fromFile = fromSquare % 8;
            int toRank = toSquare / 8;
            int toFile = toSquare % 8;

            // Check for wrapping
            if (!isDiagonal) {
                if (offset == -8 || offset == 8) { // Vertical moves
                    if (fromFile != toFile) break; // Wrapped around
                } else { // Horizontal moves
                    if (fromRank != toRank) break; // Wrapped around
                }
            } else { // Diagonal wrapping check
                if (abs(toRank - fromRank) != abs(toFile - fromFile)) break;
            }

            // Check if blocked by own pieces
            if (occupancy & (1ULL << toSquare)) break;

            int capturedPieceType = -1;
            if (enemyOccupancy & (1ULL << toSquare)) {
                capturedPieceType = board.getPieceOnSquare(toSquare);
                moves.emplace_back(fromSquare, toSquare, pieceType, capturedPieceType);
                break; // Stop on capture
            }

            // Add move
            moves.emplace_back(fromSquare, toSquare, pieceType, -1);
            toSquare += offset; // Move to next square
        }
    }
};

#endif
