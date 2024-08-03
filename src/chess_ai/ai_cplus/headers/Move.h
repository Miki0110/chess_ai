#ifndef MOVE_H
#define MOVE_H

struct Move {
    int fromSquare;
    int toSquare;
    int pieceType;
    int capturedPieceType;
    bool isPromotion;
    bool isEnPassant;
    bool isCastling;

    Move(int from, int to, int piece, int captured = -1, bool promotion = false, bool enPassant = false, bool castling = false)
        : fromSquare(from), toSquare(to), pieceType(piece), capturedPieceType(captured), 
          isPromotion(promotion), isEnPassant(enPassant), isCastling(castling) {}
};

// Function to convert board index to chess notation
std::string convertIndexToNotation(int index) {
    int file = index % 8;
    int rank = index / 8;
    char fileChar = 'a' + file;
    char rankChar = '1' + rank;
    return std::string(1, fileChar) + rankChar;
}

// Function to convert Move struct to chess notation
std::string convertMoveToNotation(const Move& move, char promotionPiece = 'Q') {
    // Handle castling moves
    if (move.isCastling) {
        if (move.toSquare == move.fromSquare + 2) return "O-O"; // Kingside castling
        if (move.toSquare == move.fromSquare - 2) return "O-O-O"; // Queenside castling
    }

    std::string fromNotation = convertIndexToNotation(move.fromSquare);
    std::string toNotation = convertIndexToNotation(move.toSquare);

    std::string moveNotation;

    // Handle captures
    if (move.capturedPieceType != -1 || move.isEnPassant) {
        moveNotation = fromNotation + std::string("x") + toNotation;
    } else {
        moveNotation = fromNotation + toNotation;
    }

    // Handle promotions
    if (move.isPromotion) {
        moveNotation += "=" + std::string(1, promotionPiece);
    }

    return moveNotation;
}

// Convert square number to file and rank
std::pair<int, int> squareToFileRank(int square) {
    // Calculate file (column) and rank (row) from the square number
    int file = square % 8;   // 0 for 'a', 1 for 'b', ..., 7 for 'h'
    int rank = square / 8;   // 0 for '8', 1 for '7', ..., 7 for '1'

    return {file, rank};
}

// Convert file and rank to array indices in terms of 2D array
std::pair<int, int> fileRankToArrayIndices(int file, int rank) {
    // Array index with (0,0) at a8
    int rowIndex = 7 - rank;  // Convert rank to array row index
    int colIndex = file;      // File index is the same as column index

    return {rowIndex, colIndex};
}

// Convert Move to array indices and return them
std::pair<std::pair<int, int>, std::pair<int, int>> convertMoveToArrayIndices(const Move& move) {
    // Get from and to squares in file, rank format
    std::pair<int, int> fromFileRank = squareToFileRank(move.fromSquare);
    std::pair<int, int> toFileRank = squareToFileRank(move.toSquare);

    int fromFile = fromFileRank.first;
    int fromRank = fromFileRank.second;
    int toFile = toFileRank.first;
    int toRank = toFileRank.second;

    // Convert to 2D array indices
    std::pair<int, int> fromIndex = fileRankToArrayIndices(fromFile, fromRank);
    std::pair<int, int> toIndex = fileRankToArrayIndices(toFile, toRank);

    // Return the indices as pairs of (row, col)
    return std::make_pair(fromIndex, toIndex);
}
#endif

