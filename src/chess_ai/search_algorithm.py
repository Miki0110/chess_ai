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


def move_generator(board, first_color):
    # set the color
    current_color = first_color

    pos_moves = []
    # Go through the board and check possible moves
    for row in range(ROWS):
        for col in range(COLS):
            square = board[row][col]
            if square.has_team_piece(current_color):
                square.piece.calc_moves(board, row, col)
                pos_moves.extend(square.piece.moves)

    return pos_moves