from src.chess_ai.eval_functions import *
from src.constants import *

import copy


def minimax(depth, board, alpha, beta, player_color, maximizing_player=True):
    # When we've found the best solution
    if depth == 0:
        return evaluate_board(board), None

    best_move = []
    if maximizing_player:
        possible_moves = move_generator(board, player_color)
        max_score = -float('inf')
        for move in possible_moves:
            temp_board = copy.deepcopy(board)
            # Move the piece
            temp_board.move_piece(move[0], move[1])
            # Repeat the process
            score, new_move = minimax(depth - 1, temp_board, alpha, beta, player_color, False)
            # Undo the move we just did
            #board.undo_move()
            # Set the scores
            if max_score < score:
                max_score = score
                best_move = move
                alpha = max(alpha, max_score)
                # Prune if the alpha is bigger than beta
                if beta <= alpha:
                    break
                #print('alpha: ', alpha)
        return max_score, best_move
    else:
        min_score = float('inf')
        possible_moves = move_generator(board, -1*player_color)
        for move in possible_moves:
            temp_board = copy.deepcopy(board)
            # Move the piece
            temp_board.move_piece(move[0], move[1])
            # Repeat the process
            score, _ = minimax(depth - 1, temp_board, alpha, beta, player_color, True)
            # Undo the move we just did
            #board.undo_move()
            # Set the scores
            if min_score >= score:
                min_score = score
                beta = min(beta, min_score)
                if beta <= alpha:
                    break
                #print('beta: ', beta)
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
    pos_moves = np.empty((1, 2, 2), dtype=int)
    for index in indexes:
        # Get the possible moves of the piece found at the given index
        moves = chess_board.get_valid_moves(index)
        # In case the piece can't move
        if len(moves) == 0:
            continue
        pos_moves = np.concatenate((pos_moves, moves))
    # Since the first contains empty positions i slice it away
    return pos_moves[1:]