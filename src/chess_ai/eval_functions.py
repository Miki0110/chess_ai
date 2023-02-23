from src.constants import *
from src.game_engine.pieces import *


def evaluate_board(board, player_color):
    """
    Evaluates a given chess board and returns a score representing the
    advantage for the current player.
    """

    # Initialize the score
    score = 0

    # Evaluate material balance
    for row in range(ROWS):
        for col in range(COLS):
            piece = board[row][col].piece
            if piece is None:
                continue
            score += piece.ret_value(player_color)
            if isinstance(piece, King):
                if piece.color == player_color:
                    player_king = board[row][col]
                else:
                    enemy_king = board[row][col]

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
    player_king_pos = (player_king.row, player_king.col)
    enemy_king_pos = (enemy_king.row, enemy_king.col)

    # For the players king position
    for i in range(player_king_pos[0]-3, player_king_pos[0]+3):
        if ROWS <= i >= 0: # Make sure we are still on the board
            continue
        for j in range(player_king_pos[1]-3, player_king_pos[1]+3):
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
