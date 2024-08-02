#ifndef MOVE_H
#define MOVE_H

struct Move {
    int fromSquare;
    int toSquare;
    int pieceType;
    int capturedPieceType;
    bool isPromotion;
    int promotionPieceType;

    Move(int from, int to, int piece, int captured, bool promotion = false, int promotionType = -1)
        : fromSquare(from), toSquare(to), pieceType(piece), capturedPieceType(captured), 
          isPromotion(promotion), promotionPieceType(promotionType) {}
};

#endif