from src.constants import *
from src.chess_ai.board_representation import *

# 1 = P, 2 = R, 3 = N, 4 = B, 5 = K, 6 = Q
piece_values = {1: 1,  # Pawn
               2: 5,  # Rook
               3: 3,  # Knight
               4: 3,  # Bishop
               5: 1000000,  # King (arbitrary high value to avoid being captured)
               6: 9,  # Queen
               -1: -1,  # Pawn
               -2: -5,  # Rook
               -3: -3,  # Knight
               -4: -3,  # Bishop
               -5: -1000000,  # King (arbitrary high value to avoid being captured)
               -6: -9,  # Queen
               }


def evaluate_board(board, current_side):
    """
    Evaluates a given chess board and returns a score representing the
    advantage for the current player.
    """

    # Initialize the score
    score = 0
    # create boolean masks for each type of piece
    piece_masks = {
        1: board == 1,  # w_pawn_mask
        2: board == 2,  # w_rook_mask
        3: board == 3,  # w_knight_mask
        4: board == 4,  # w_bishop_mask
        5: board == 5,  # w_king_mask
        6: board == 6,  # w_queen_mask
        -1: board == -1,  # b_pawn_mask
        -2: board == -2,  # b_rook_mask
        -3: board == -3,  # b_knight_mask
        -4: board == -4,  # b_bishop_mask
        -5: board == -5,  # b_king_mask
        -6: board == -6,  # b_queen_mask
    }

    # Calculate the material balance
    material_balance = sum(piece_values[piece] * piece_masks[piece].sum() for piece in piece_values)

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
    white_king_pos = np.where(piece_masks[5] is True)
    black_king_pos = np.where(piece_masks[-5] is True)

    # For the players king position
    for i in range(white_king_pos[0]-3, white_king_pos[0]+3):
        if ROWS <= i >= 0: # Make sure we are still on the board
            continue
        for j in range(white_king_pos[1]-3, white_king_pos[1]+3):
            if COLS <= j >= 0:
                continue
            piece = board[i][j]
            if piece.has_team_piece(player_color):
                score += 0.2
    # For the Enemies king position
    for i in range(enemy_king_pos[0] - 3, enemy_king_pos[0] + 3):
        if ROWS <= i >= 0:  # Make sure we are still on the board
            continue
        for j in range(enemy_king_pos[1] - 3, enemy_king_pos[1] + 3):
            if COLS <= j >= 0:
                continue
            piece = board[i][j]
            if not piece.has_team_piece(player_color):
                score += 0.2

    return score
