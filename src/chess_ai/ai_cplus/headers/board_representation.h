#ifndef BOARD_REPRESENTATION_H
#define BOARD_REPRESENTATION_H

#include <cstdint>
#include <array>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

enum PieceType {
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK,
    WHITE_QUEEN, WHITE_KING, BLACK_PAWN, BLACK_KNIGHT,
    BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING
};

struct Move {
    int fromSquare;
    int toSquare;
    int pieceType;
    int capturedPieceType;
    bool isPromotion;
    int promotionPieceType;
};

class ChessEngine {
public:
    ChessEngine(const std::string& fen) {
        parseFEN(fen);
    }

    void parseFEN(const std::string& fen) {
        pieceBitboards.fill(0);
        whitePieces = blackPieces = allPieces = 0;

        std::stringstream ss(fen);
        std::string boardPart;
        ss >> boardPart;

        int square = 56; // Start from a8

        for (char c : boardPart) {
            if (c == '/') {
                square -= 16; // Move to the next row
            } else if (isdigit(c)) {
                square += c - '0';
            } else {
                int pieceType = -1;
                switch (c) {
                    case 'P': pieceType = WHITE_PAWN; break;
                    case 'N': pieceType = WHITE_KNIGHT; break;
                    case 'B': pieceType = WHITE_BISHOP; break;
                    case 'R': pieceType = WHITE_ROOK; break;
                    case 'Q': pieceType = WHITE_QUEEN; break;
                    case 'K': pieceType = WHITE_KING; break;
                    case 'p': pieceType = BLACK_PAWN; break;
                    case 'n': pieceType = BLACK_KNIGHT; break;
                    case 'b': pieceType = BLACK_BISHOP; break;
                    case 'r': pieceType = BLACK_ROOK; break;
                    case 'q': pieceType = BLACK_QUEEN; break;
                    case 'k': pieceType = BLACK_KING; break;
                }
                setPiece(pieceType, square);
                square++;
            }
        }
        updateOccupancy();
    }

    void displayBoard() const {
        for (int rank = 7; rank >= 0; --rank) {
            for (int file = 0; file < 8; ++file) {
                int square = rank * 8 + file;
                char piece = '.';
                for (int i = 0; i < 12; ++i) {
                    if (pieceBitboards[i] & (1ULL << square)) {
                        switch (i) {
                            case WHITE_PAWN: piece = 'P'; break;
                            case WHITE_KNIGHT: piece = 'N'; break;
                            case WHITE_BISHOP: piece = 'B'; break;
                            case WHITE_ROOK: piece = 'R'; break;
                            case WHITE_QUEEN: piece = 'Q'; break;
                            case WHITE_KING: piece = 'K'; break;
                            case BLACK_PAWN: piece = 'p'; break;
                            case BLACK_KNIGHT: piece = 'n'; break;
                            case BLACK_BISHOP: piece = 'b'; break;
                            case BLACK_ROOK: piece = 'r'; break;
                            case BLACK_QUEEN: piece = 'q'; break;
                            case BLACK_KING: piece = 'k'; break;
                        }
                    }
                }
                std::cout << piece << ' ';
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    std::vector<Move> generateLegalMoves(bool whiteToMove) {
        std::vector<Move> moves;

        uint64_t occupancy = whiteToMove ? whitePieces : blackPieces;
        uint64_t enemyOccupancy = whiteToMove ? blackPieces : whitePieces;

        for (int pieceType = whiteToMove ? WHITE_PAWN : BLACK_PAWN; pieceType <= (whiteToMove ? WHITE_KING : BLACK_KING); ++pieceType) {
            uint64_t pieceBitboard = pieceBitboards[pieceType];
            while (pieceBitboard) {
                int fromSquare = __builtin_ctzll(pieceBitboard); // Get least significant bit index
                generatePieceMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy);
                pieceBitboard &= pieceBitboard - 1; // Clear least significant bit
            }
        }

        return moves;
    }

private:
    std::array<uint64_t, 12> pieceBitboards;  // 12 bitboards for each piece type
    uint64_t whitePieces, blackPieces;        // Overall occupancy for white and black
    uint64_t allPieces;                       // Overall occupancy for all pieces

    void updateOccupancy() {
        whitePieces = blackPieces = allPieces = 0;
        for (int i = WHITE_PAWN; i <= WHITE_KING; ++i) {
            whitePieces |= pieceBitboards[i];
        }
        for (int i = BLACK_PAWN; i <= BLACK_KING; ++i) {
            blackPieces |= pieceBitboards[i];
        }
        allPieces = whitePieces | blackPieces;
    }

    void setPiece(int pieceType, int square) {
        pieceBitboards[pieceType] |= (1ULL << square);
        updateOccupancy();
    }

    void clearSquare(int square) {
        for (auto& bitboard : pieceBitboards) {
            bitboard &= ~(1ULL << square);
        }
        updateOccupancy();
    }

    void generatePieceMoves(int fromSquare, int pieceType, std::vector<Move>& moves, uint64_t occupancy, uint64_t enemyOccupancy) {
        // For simplicity, let's assume we only generate pawn moves as an example
        if (pieceType == WHITE_PAWN || pieceType == BLACK_PAWN) {
            generatePawnMoves(fromSquare, pieceType, moves, occupancy, enemyOccupancy);
        }
        // Implement other piece moves here...
    }

    void generatePawnMoves(int fromSquare, int pieceType, std::vector<Move>& moves, uint64_t occupancy, uint64_t enemyOccupancy) {
        int direction = (pieceType == WHITE_PAWN) ? 8 : -8; // Move direction for white and black pawns

        int toSquare = fromSquare + direction;

        if (!(occupancy & (1ULL << toSquare))) { // Check if the square is empty
            moves.push_back(Move{fromSquare, toSquare, pieceType, -1, false, -1}); // Add normal move

            // Check for double move from starting rank
            if ((pieceType == WHITE_PAWN && fromSquare >= 8 && fromSquare <= 15) ||
                (pieceType == BLACK_PAWN && fromSquare >= 48 && fromSquare <= 55)) {
                int doubleToSquare = toSquare + direction;
                if (!(occupancy & (1ULL << doubleToSquare))) {
                    moves.push_back(Move{fromSquare, doubleToSquare, pieceType, -1, false, -1});
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
                    if (pieceBitboards[i] & (1ULL << captureSquare)) {
                        capturedPieceType = i;
                        break;
                    }
                }
                moves.push_back(Move{fromSquare, captureSquare, pieceType, capturedPieceType, false, -1});
            }
        }
    }
};

#endif
