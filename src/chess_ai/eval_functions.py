import sys
import os
# Add src to the system path for module imports
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from constants import *
from chess_ai.board_representation import *
from chess_ai.evaluation_values import *


def evaluate_board(chess_board):
    """
    Evaluates a given chess board and returns a score representing the
    advantage for the white player.
    """
    # Initialize the score
    score = 0
    # create boolean masks for each type of piece
    piece_masks = {
        1: chess_board.board == 1,  # w_pawn_mask
        2: chess_board.board == 2,  # w_rook_mask
        3: chess_board.board == 3,  # w_knight_mask
        4: chess_board.board == 4,  # w_bishop_mask
        5: chess_board.board == 5,  # w_king_mask
        6: chess_board.board == 6,  # w_queen_mask
        -1: chess_board.board == -1,  # b_pawn_mask
        -2: chess_board.board == -2,  # b_rook_mask
        -3: chess_board.board == -3,  # b_knight_mask
        -4: chess_board.board == -4,  # b_bishop_mask
        -5: chess_board.board == -5,  # b_king_mask
        -6: chess_board.board == -6,  # b_queen_mask
    }
    # Check game stage
    early_stage = True if np.sum(chess_board.board != 0) > 22 else False

    # Calculate the material balance
    if early_stage:
        material_balance = sum(piece_values[piece] * piece_masks[piece].sum() +
                               np.sum(piece_masks[piece]*mg_value_tables[piece]) for piece in piece_values)
    else:
        material_balance = sum(piece_values[piece] * piece_masks[piece].sum() +
                               np.sum(piece_masks[piece] * eg_value_tables[piece]) for piece in piece_values)
    score += material_balance


    # Evaluate pawn structure
    """
    for i in range(ROWS):
        for j in range(COLS):
            piece = board[i][j].piece
            if piece is None:
                continue
            if isinstance(piece, Pawn):
                # Penalize isolated pawns
                if not any(isinstance(board[k][j].piece, Pawn) for k in range(ROWS) if k != i):
                    score -= 0.1
                # Penalize doubled pawns
                if isinstance(board[i-1][j].piece, Pawn) or isinstance(board[i+1][j].piece, Pawn):
                    score -= 0.1
    """
    # Evaluate king safety
    white_king_pos = np.where(piece_masks[5] == True)
    black_king_pos = np.where(piece_masks[-5] == True)

    # If the king is missing, we cannot do this
    if len(black_king_pos[0]) == 0 or len(white_king_pos[0]) == 0:
        return score

    # For the white king position
    for i in range(white_king_pos[0][0]-2, white_king_pos[0][0]+2):
        if ROWS <= i >= 0:  # Make sure we are still on the board
            continue
        for j in range(white_king_pos[1][0]-2, white_king_pos[1][0]+2):  # Last zero is because numpy is being annoying
            if COLS <= j >= 0:
                continue
            if chess_board.has_ally((i, j), 1):
                # Save the score
                score += 0.2
    # Black side
    for i in range(black_king_pos[0][0]-2, black_king_pos[0][0]+2):
        if ROWS <= i >= 0:  # Make sure we are still on the board
            continue
        for j in range(black_king_pos[1][0]-2, black_king_pos[1][0]+2):
            if COLS <= j >= 0:
                continue
            if chess_board.has_ally((i, j), -1):
                score -= 0.2

    return score
