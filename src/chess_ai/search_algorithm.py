from src.chess_ai.eval_functions import *
import time


def timeit(n=1):
    """
    Wrapper used to time the execution time of functions
    :param n: int, sets the amount of runs
    :return: returns the function values
    """
    def decorator(func):
        def wrapper(*args, **kwargs):
            total_time = 0
            for i in range(n):
                start_time = time.time()
                result = func(*args, **kwargs)
                end_time = time.time()
                total_time += (end_time - start_time)
            print(f"Function '{func.__name__}' took {total_time:.5f} seconds to execute over {n} runs.")
            return result
        return wrapper
    return decorator


# TODO: Introduce a hashtable and cache expensive computation
def minimax(depth, board, alpha, beta, maximizing_player=True):
    """
    Mini max function with alpha beta pruning
    :param depth: The depth the function should search to
    :param board: The board class
    :param alpha: alpha value, set to -float('inf') when calling
    :param beta: beta value, set to float('inf') when calling
    :param maximizing_player: set to True when the player is white and false when the player is black
    :return: board score, best move
    """
    # When we've found the best solution
    if depth == 0:
        return evaluate_board(board), None

    best_move = []
    if maximizing_player:
        possible_moves = move_generator(board, 1)
        max_score = -float('inf')
        if len(possible_moves) == 0:
            return -10000, None
        for move in possible_moves:
            # Move the piece
            board.move_piece(move[0], move[1])
            # Repeat the process
            score, new_move = minimax(depth - 1, board, alpha, beta, False)
            # Undo the move we just did
            board.undo_move()

            # Set the scores
            if max_score < score:
                max_score = score
                best_move = move
                alpha = max(alpha, max_score)
                # Prune if the alpha is bigger than beta
                if beta <= alpha:
                    break
        return max_score, best_move
    else:
        min_score = float('inf')
        possible_moves = move_generator(board, -1)
        if len(possible_moves) == 0:
            return 10000, None
        for move in possible_moves:
            # Move the piece
            board.move_piece(move[0], move[1])
            # Repeat the process
            score, new_move = minimax(depth - 1, board, alpha, beta, True)
            # Undo the move we just did
            board.undo_move()
            # Set the scores
            if min_score >= score:
                min_score = score
                best_move = move
                beta = min(beta, min_score)
                if beta <= alpha:
                    break
        return min_score, best_move


def move_generator(chess_board, current_color):
    # Pieces
    if current_color == 1:
        piece_mask = chess_board.board > 0
    else:
        piece_mask = chess_board.board < 0

    # Get all the indexes indicating players
    indexes = np.argwhere(piece_mask)

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