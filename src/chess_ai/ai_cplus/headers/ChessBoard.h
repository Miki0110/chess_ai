#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <cstdint>
#include <array>
#include <string>
#include <sstream>
#include <iostream>
#include <random>
#include <limits>
#include "Move.h"

enum PieceType {
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK,
    WHITE_QUEEN, WHITE_KING, BLACK_PAWN, BLACK_KNIGHT,
    BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING
};

class ChessBoard {
public:
    // Constructors
    ChessBoard(const std::string& fen) {
        initializeZobristTable();
        parseFEN(fen);
        currentHash = calculateHash(); // Calculate initial hash from FEN
    }

    // Parse FEN string to set up the board
    void parseFEN(const std::string& fen) {
        pieceBitboards.fill(0);
        whitePieces = blackPieces = allPieces = 0;

        std::stringstream ss(fen);
        std::string boardPart, activeColorPart, castlingPart, enPassantPart;
        ss >> boardPart >> activeColorPart >> castlingPart >> enPassantPart;

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

        parseCastlingRights(castlingPart);
        parseEnPassant(enPassantPart);
        updateOccupancy();

        // DEBUG print the string parts
        //std::cout << "FEN: " << fen << std::endl;
        //std::cout << "Board: " << boardPart << std::endl;
        
        //std::cout << "Castling: " << castlingPart << std::endl;
        //std::cout << "En Passant: " << enPassantPart << std::endl;
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

        // Update the Zobrist hash for the move
        currentHash ^= zobristTable[move.pieceType][move.fromSquare]; // Remove piece from original square
        currentHash ^= zobristTable[move.pieceType][move.toSquare];   // Place piece on new square

        if (move.capturedPieceType != -1) {
            clearSquare(move.toSquare);
            currentHash ^= zobristTable[move.capturedPieceType][move.toSquare]; // Remove captured piece
        }

        if (move.isPromotion) {
            clearSquare(move.toSquare);
            int promotionPieceType = (move.pieceType == WHITE_PAWN) ? WHITE_QUEEN : BLACK_QUEEN;
            setPiece(promotionPieceType, move.toSquare);

            // Update the hash for promotion
            currentHash ^= zobristTable[move.pieceType][move.toSquare];        // Remove promoted pawn
            currentHash ^= zobristTable[promotionPieceType][move.toSquare]; // Add promoted queen
        }

        if (move.isCastling) {
            // Handle castling moves
            if (move.toSquare == move.fromSquare + 2) { // Kingside castling
                moveRookForCastling(move.toSquare - 1, move.toSquare + 1); // Move rook to king's left
            } else if (move.toSquare == move.fromSquare - 2) { // Queenside castling
                moveRookForCastling(move.toSquare + 1, move.toSquare - 2); // Move rook to king's right
            }
        }

        if (move.isEnPassant) {
            // Handle en passant capture
            int capturedPawnSquare = move.toSquare + (move.pieceType == WHITE_PAWN ? -8 : 8);
            clearSquare(capturedPawnSquare);
        }

        updateCastlingRights(move); // Update castling rights based on the move
        updateEnPassantSquare(move); // Update en passant square for potential en passant capture
        updateOccupancy();
    }

    // Undo a move on the board
    void undoMove(const Move& move) {
        clearSquare(move.toSquare);
        setPiece(move.pieceType, move.fromSquare);

        // Revert the Zobrist hash for the move
        currentHash ^= zobristTable[move.pieceType][move.toSquare];   // Remove piece from new square
        currentHash ^= zobristTable[move.pieceType][move.fromSquare]; // Place piece back on original square

        if (move.capturedPieceType != -1) {
            setPiece(move.capturedPieceType, move.toSquare);
            currentHash ^= zobristTable[move.capturedPieceType][move.toSquare]; // Restore captured piece
        }

        if (move.isPromotion) {
            clearSquare(move.fromSquare);
            int originalPawnType = (move.pieceType == WHITE_QUEEN) ? WHITE_PAWN : BLACK_PAWN;
            setPiece(originalPawnType, move.fromSquare);  // Assuming promotion was from a pawn to another piece

            // Revert the hash for promotion
            currentHash ^= zobristTable[move.pieceType][move.toSquare]; // Remove promoted queen
            currentHash ^= zobristTable[originalPawnType][move.toSquare];          // Restore original pawn
        }

        if (move.isCastling) {
            // Handle undoing castling moves
            if (move.toSquare == move.fromSquare + 2) { // Kingside castling
                moveRookForCastling(move.toSquare + 1, move.toSquare - 1); // Move rook back to original position
            } else if (move.toSquare == move.fromSquare - 2) { // Queenside castling
                moveRookForCastling(move.toSquare - 2, move.toSquare + 1); // Move rook back to original position
            }
        }

        if (move.isEnPassant) {
            // Handle undoing en passant capture
            int capturedPawnSquare = move.toSquare + (move.pieceType == WHITE_PAWN ? -8 : 8);
            setPiece((move.pieceType == WHITE_PAWN) ? BLACK_PAWN : WHITE_PAWN, capturedPawnSquare);
        }

        restorePreviousCastlingRights(); // Restore castling rights to their state before the move
        restorePreviousEnPassantSquare(); // Restore en passant square to its previous state
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

    // Get the current Zobrist hash of the board
    uint64_t hash() const {
        return currentHash;
    }

    // Get the current en passant square
    int getEnPassantSquare() const {
        return enPassantSquare;
    }

    // Check castling rights
    bool canCastleKingside(int color) const {
        return (color == WHITE_KING) ? whiteKingsideCastle : blackKingsideCastle;
    }

    bool canCastleQueenside(int color) const {
        return (color == WHITE_KING) ? whiteQueensideCastle : blackQueensideCastle;
    }

    // Get the piece on a specific square
    int getPieceOnSquare(int square) const {
        for (int i = 0; i < 12; ++i) {
            if (pieceBitboards[i] & (1ULL << square)) {
                return i;
            }
        }
        return -1; // Empty square
    }
    
private:
    std::array<uint64_t, 12> pieceBitboards;  // 12 bitboards for each piece type
    uint64_t whitePieces, blackPieces;        // Overall occupancy for white and black
    uint64_t allPieces;                       // Overall occupancy for all pieces

    std::array<std::array<uint64_t, 64>, 12> zobristTable; // Zobrist table for hashing
    uint64_t currentHash; // Current Zobrist hash of the board

    // Castling rights
    bool whiteKingsideCastle = false;
    bool whiteQueensideCastle = false;
    bool blackKingsideCastle = false;
    bool blackQueensideCastle = false;

    // En passant square (-1 if not applicable)
    int enPassantSquare = -1;

    // Previous state tracking
    bool previousWhiteKingsideCastle;
    bool previousWhiteQueensideCastle;
    bool previousBlackKingsideCastle;
    bool previousBlackQueensideCastle;
    int previousEnPassantSquare;

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

    // Initialize the Zobrist table with random numbers
    void initializeZobristTable() {
        std::mt19937_64 rng(12345); // Seed for reproducibility
        std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());

        for (int piece = 0; piece < 12; ++piece) {
            for (int square = 0; square < 64; ++square) {
                zobristTable[piece][square] = dist(rng);
            }
        }
    }

    // Calculate the initial Zobrist hash of the board
    uint64_t calculateHash() const {
        uint64_t hash = 0;
        for (int piece = 0; piece < 12; ++piece) {
            uint64_t bitboard = pieceBitboards[piece];
            while (bitboard) {
                int square = __builtin_ctzll(bitboard); // Find the least significant bit set
                hash ^= zobristTable[piece][square];
                bitboard &= bitboard - 1; // Clear the least significant bit set
            }
        }
        return hash;
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

    // Parse castling rights from FEN
    void parseCastlingRights(const std::string& castlingPart) {
        whiteKingsideCastle = castlingPart.find('K') != std::string::npos;
        whiteQueensideCastle = castlingPart.find('Q') != std::string::npos;
        blackKingsideCastle = castlingPart.find('k') != std::string::npos;
        blackQueensideCastle = castlingPart.find('q') != std::string::npos;
    }

    // Parse en passant square from FEN
    void parseEnPassant(const std::string& enPassantPart) {
        if (enPassantPart != "-") {
            int file = enPassantPart[0] - 'a';
            int rank = enPassantPart[1] - '1';
            enPassantSquare = rank * 8 + file;
        } else {
            enPassantSquare = -1;
        }
    }

    // Move the rook for castling
    void moveRookForCastling(int fromSquare, int toSquare) {
        for (int pieceType = WHITE_ROOK; pieceType <= BLACK_ROOK; pieceType += 6) {
            if (pieceBitboards[pieceType] & (1ULL << fromSquare)) {
                clearSquare(fromSquare);
                setPiece(pieceType, toSquare);
                break;
            }
        }
    }

    // Update castling rights based on the move
    void updateCastlingRights(const Move& move) {
        previousWhiteKingsideCastle = whiteKingsideCastle;
        previousWhiteQueensideCastle = whiteQueensideCastle;
        previousBlackKingsideCastle = blackKingsideCastle;
        previousBlackQueensideCastle = blackQueensideCastle;

        // If king moves, both castling rights are lost
        if (move.pieceType == WHITE_KING) {
            whiteKingsideCastle = false;
            whiteQueensideCastle = false;
        } else if (move.pieceType == BLACK_KING) {
            blackKingsideCastle = false;
            blackQueensideCastle = false;
        }

        // If a rook moves or is captured, that side's castling right is lost
        if (move.pieceType == WHITE_ROOK || move.capturedPieceType == WHITE_ROOK) {
            if (move.fromSquare == 0 || move.toSquare == 0) whiteQueensideCastle = false;
            if (move.fromSquare == 7 || move.toSquare == 7) whiteKingsideCastle = false;
        } else if (move.pieceType == BLACK_ROOK || move.capturedPieceType == BLACK_ROOK) {
            if (move.fromSquare == 56 || move.toSquare == 56) blackQueensideCastle = false;
            if (move.fromSquare == 63 || move.toSquare == 63) blackKingsideCastle = false;
        }
    }

    // Restore previous castling rights (for undoing moves)
    void restorePreviousCastlingRights() {
        whiteKingsideCastle = previousWhiteKingsideCastle;
        whiteQueensideCastle = previousWhiteQueensideCastle;
        blackKingsideCastle = previousBlackKingsideCastle;
        blackQueensideCastle = previousBlackQueensideCastle;
    }

    // Update en passant square for potential en passant capture
    void updateEnPassantSquare(const Move& move) {
        previousEnPassantSquare = enPassantSquare;
        if ((move.pieceType == WHITE_PAWN || move.pieceType == BLACK_PAWN) &&
            std::abs(move.toSquare - move.fromSquare) == 16) {
            enPassantSquare = (move.fromSquare + move.toSquare) / 2;
        } else {
            enPassantSquare = -1;
        }
    }

    // Restore previous en passant square (for undoing moves)
    void restorePreviousEnPassantSquare() {
        enPassantSquare = previousEnPassantSquare;
    }

    
};

#endif


