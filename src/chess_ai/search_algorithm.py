from src.chess_ai.eval_functions import *
from src.constants import *

import copy


def minimax(depth, board, alpha, beta, player_color, maximizing_player=True):
    # When we've found the best solution
    if depth == 0:
        return evaluate_board(board.squares, player_color), None

    best_move = []

    if maximizing_player:
        possible_moves = move_generator(board.squares, player_color)
        max_score = -100000
        if len(possible_moves) == 0:
            return max_score
        for move in possible_moves:
            temp_board = copy.deepcopy(board)
            # First we find the piece related to the move
            piece = temp_board.squares[move.initial.row][move.initial.col].piece
            # Move the piece
            temp_board.move(piece, move)
            # Repeat the process
            score, new_move = minimax(depth - 1, temp_board, alpha, beta, player_color, False)
            if max_score <= score:
                max_score = score
                best_move = move
            alpha = max(alpha, max_score)
            if beta <= alpha:
                break
        return max_score, best_move
    else:
        min_score = 100000
        enemy_color = 'white' if player_color == 'black' else 'black'
        possible_moves = move_generator(board.squares, enemy_color)
        if len(possible_moves) == 0:
            return min_score
        for move in possible_moves:
            temp_board = copy.deepcopy(board)
            # First we find the piece related to the move
            piece = temp_board.squares[move.initial.row][move.initial.col].piece
            # Move the piece
            temp_board.move(piece, move)
            # Repeat the process
            score, _ = minimax(depth - 1, board, alpha, beta, player_color, True)

            if min_score >= score:
                min_score = score
            beta = min(beta, min_score)
            if beta <= alpha:
                break
        return min_score, None


def move_generator(chess_board, current_color):
    # Pieces
    if current_color == 1:
        piece_mask = chess_board.board > 0
    else:
        piece_mask = chess_board.board < 0

    # Get all the indexes indicating players
    indexes = np.transpose(np.where(piece_mask))

    # Go through the possible moves for each piece
    pos_moves = []
    for index in indexes:
        moves = chess_board.get_valid_moves(index)
        pos_moves.append([index, moves])
    return pos_moves