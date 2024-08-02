#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <cstdint>
#include <array>
#include <string>
#include <sstream>
#include <iostream>
#include "Move.h"

enum PieceType {
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK,
    WHITE_QUEEN, WHITE_KING, BLACK_PAWN, BLACK_KNIGHT,
    BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING
};

class ChessBoard {
public:
    ChessBoard(const std::string& fen) {
        parseFEN(fen);
    }

    // Parse FEN string to set up the board
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
                int pieceType = charToPieceType(c);
                setPiece(pieceType, square);
                square++;
            }
        }
        updateOccupancy();
    }

    // Display the board in a human-readable format
    void displayBoard() const {
        for (int rank = 7; rank >= 0; --rank) {
            for (int file = 0; file < 8; ++file) {
                int square = rank * 8 + file;
                char piece = '.';
                for (int i = 0; i < 12; ++i) {
                    if (pieceBitboards[i] & (1ULL << square)) {
                        piece = pieceTypeToChar(i);
                    }
                }
                std::cout << piece << ' ';
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // Apply a move to the board
    void applyMove(const Move& move) {
        clearSquare(move.fromSquare);
        setPiece(move.pieceType, move.toSquare);
        if (move.capturedPieceType != -1) {
            clearSquare(move.toSquare);
        }

        if (move.isPromotion) {
            clearSquare(move.toSquare);
            setPiece(move.promotionPieceType, move.toSquare);
        }

        updateOccupancy();
    }

    // Undo a move on the board
    void undoMove(const Move& move) {
        clearSquare(move.toSquare);
        setPiece(move.pieceType, move.fromSquare);
        if (move.capturedPieceType != -1) {
            setPiece(move.capturedPieceType, move.toSquare);
        }

        updateOccupancy();
    }

    // Get bitboard for a specific piece type
    uint64_t getPieceBitboard(int pieceType) const {
        return pieceBitboards[pieceType];
    }

    // Set a piece at a specific square
    void setPiece(int pieceType, int square) {
        pieceBitboards[pieceType] |= (1ULL << square);
        updateOccupancy();
    }

    // Clear a piece from a specific square
    void clearSquare(int square) {
        for (auto& bitboard : pieceBitboards) {
            bitboard &= ~(1ULL << square);
        }
        updateOccupancy();
    }

    // Get occupancy bitboards
    uint64_t getWhiteOccupancy() const {
        return whitePieces;
    }

    uint64_t getBlackOccupancy() const {
        return blackPieces;
    }

    uint64_t getAllOccupancy() const {
        return allPieces;
    }

private:
    std::array<uint64_t, 12> pieceBitboards;  // 12 bitboards for each piece type
    uint64_t whitePieces, blackPieces;        // Overall occupancy for white and black
    uint64_t allPieces;                       // Overall occupancy for all pieces

    // Update the occupancy bitboards
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

    // Convert a piece type to a character for display
    char pieceTypeToChar(int pieceType) const {
        switch (pieceType) {
            case WHITE_PAWN: return 'P';
            case WHITE_KNIGHT: return 'N';
            case WHITE_BISHOP: return 'B';
            case WHITE_ROOK: return 'R';
            case WHITE_QUEEN: return 'Q';
            case WHITE_KING: return 'K';
            case BLACK_PAWN: return 'p';
            case BLACK_KNIGHT: return 'n';
            case BLACK_BISHOP: return 'b';
            case BLACK_ROOK: return 'r';
            case BLACK_QUEEN: return 'q';
            case BLACK_KING: return 'k';
            default: return '.';
        }
    }

    // Convert a character to a piece type
    int charToPieceType(char c) const {
        switch (c) {
            case 'P': return WHITE_PAWN;
            case 'N': return WHITE_KNIGHT;
            case 'B': return WHITE_BISHOP;
            case 'R': return WHITE_ROOK;
            case 'Q': return WHITE_QUEEN;
            case 'K': return WHITE_KING;
            case 'p': return BLACK_PAWN;
            case 'n': return BLACK_KNIGHT;
            case 'b': return BLACK_BISHOP;
            case 'r': return BLACK_ROOK;
            case 'q': return BLACK_QUEEN;
            case 'k': return BLACK_KING;
            default: return -1;
        }
    }
};

#endif
