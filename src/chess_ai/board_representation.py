import numpy as np
import time

# Dictionary for the string values of each piece value
piece_values = {1: 'P',  # Pawn
                2: 'R',  # Rook
                3: 'N',  # Knight
                4: 'B',  # Bishop
                5: 'K',  # King (arbitrary high value to avoid being captured)
                6: 'Q',  # Queen
                -1: 'p',  # Pawn
                -2: 'r',  # Rook
                -3: 'n',  # Knight
                -4: 'b',  # Bishop
                -5: 'k',  # King (arbitrary high value to avoid being captured)
                -6: 'q',  # Queen
                }
# Dictionary for getting the alpha values
ALPHACOLS = {0: 'a', 1: 'b', 2: 'c', 3: 'd', 4: 'e', 5: 'f', 6: 'g', 7: 'h'}

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


# TODO: Fix en passant to only set value when there is a possible pawn
class ChessBoard:
    def __init__(self, FEN_string):
        self.en_passant = None
        self.black_castle = [True, True]  # Queens-, Kings -side
        self.white_castle = [True, True]  # Queens-, Kings -side
        self.current_player = 1  # 1 = white, -1 = black
        self.board = self.set_board(FEN_string)

        # Stack for minimax
        self.board_state_stack = []
        self.black_castle_stack = []  # Queens-, Kings -side
        self.white_castle_stack = []  # Queens-, Kings -side

    def set_board(self, FEN):
        """
        Takes a FEN string and sets a numpy array up containing the board pieces from that
        """
        # rnbqkbnr/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQKBNR w KQ g4 1 1
        # Split the string into each of the information types
        fen_split = FEN.split('/')
        pieces = fen_split[:-1] + [fen_split[-1].split(' ')[0]]
        states = fen_split[-1].split(' ')[1:]
        # Create the board
        board = np.zeros((8, 8), dtype=int)
        # set the pieces
        col = 0
        for row, values in enumerate(pieces):
            values.split()
            for value in values:
                if value.isdigit():
                    col += int(value)-1
                else:
                    side = -1
                    if not value.islower():
                        side = 1
                        value = value.lower()
                    if value == 'p':
                        board[row][col] = int(1*side)
                    elif value == 'r':
                        board[row][col] = int(2*side)
                    elif value == 'n':
                        board[row][col] = int(3*side)
                    elif value == 'b':
                        board[row][col] = int(4*side)
                    elif value == 'k':
                        board[row][col] = int(5*side)
                    elif value == 'q':
                        board[row][col] = int(6*side)
                col += 1
            col = 0
        # Set the current player
        self.current_player = 1 if states[0] == 'w' else -1

        # Castling
        if states[1] == '-':
            self.white_castle = [False, False]
            self.black_castle = [False, False]
        if 'q' in states[1]:
            self.black_castle[0] = True
        if 'Q' in states[1]:
            self.white_castle[0] = True
        if 'k' in states[1]:
            self.black_castle[1] = True
        if 'K' in states[1]:
            self.white_castle[1] = True

        # En passant
        if states[2] != '-':
            ALPHACOLS = {'a': 0, 'b': 1, 'c': 2, 'd': 3, 'e': 4, 'f': 5, 'g': 6, 'h': 7}
            self.en_passant = np.array([ALPHACOLS[states[2][0]], int(states[2][1])])

        return board

    def board_to_FEN(self, side):
        """
        Function for retrieving a FEN string, used to describe position in the hashtable
        :param side: 1 for white, -1 for black
        :return: A FEN string
        """
        player = 'w' if side == 1 else 'b'

        # Replace empty squares with a number representing the count of empty squares before the next piece
        fen = ""
        for i, row in enumerate(self.board):
            new_row = ""
            # Count the distance to pieces through np.nonzero
            indexes = np.nonzero(row)[0]  # Again the 0 is because of numpy bullshittery
            # If there are no pieces we just write 8
            if len(indexes) == 0:
                new_row = '8'
            else:
                # Used for tracking length into the row
                full_num = 0
                for index in indexes:
                    num = index-full_num+1  # Distance from last piece
                    full_num += num  # Keeping track of how far in the row we are
                    # If the length is 1 we write no number
                    if num == 1:
                        new_row += piece_values[row[index]]
                    else:
                        new_row += str(num)+piece_values[row[index]]
            # Next row indicated by a backslash
            fen += new_row+'/'
        # Check for castling
        castling = ''
        if self.black_castle[0]:
            castling += 'q'
        if self.black_castle[1]:
            castling += 'k'
        if self.white_castle[0]:
            castling += 'Q'
        if self.white_castle[1]:
            castling += 'K'
        if castling == '':
            castling = '-'
        # Write in the en passant
        if self.en_passant is None:
            passant = '-'
        else:
            passant = ALPHACOLS[self.en_passant[1]]+str(self.en_passant[0])
        # Add in all the other values
        fen = fen[:-1] + f' {player} {castling} {passant}'
        return fen

    def print_board(self):
        print(self.board)

    # Check for enemy piece at pos
    def has_enemy(self, pos, side):
        x, y = pos
        piece = self.board[x][y]
        if piece == 0:
            return False
        if piece // abs(piece) == side:
            return False
        else:
            return True

    # Check for ally at pos
    def has_ally(self, pos, side):
        x, y = pos
        piece = self.board[x][y]
        if piece == 0:
            return False
        elif piece // abs(piece) != side:
            return False
        else:
            return True

    def move_piece(self, start_pos, end_pos):
        """
        Moves a piece at start pos to end pos.
        While changing values for en passant and castling
        """

        # Save the move
        self.board_state_stack.append(self.board.copy())
        self.black_castle_stack.append(self.black_castle)
        self.white_castle_stack.append(self.white_castle)

        # Retrieve the piece and start moving it
        x1, y1 = start_pos
        x2, y2 = end_pos
        piece = self.board[x1][y1]

        # Check for pawns
        if abs(piece) == 1:
            # If we opened up for an en passant
            if abs(x1-x2) > 1 and ((y2+1 <= 7 and self.board[x2][y2+1] == -1*piece) or (y2-1 >= 0 and self.board[x2][y2-1] == -1*piece)):
                self.en_passant = [x2+piece, y2]
            # Check if we did one instead
            if self.en_passant is not None:
                if (piece == 1 and x2 == 2) or (piece == -1 and x2 == 5)\
                        and end_pos[0] == self.en_passant[0] and end_pos[1] == self.en_passant[1]:
                    # Get the place of the pawn
                    enemy_pos = [x1, y2]
                    # Remove it from the board
                    self.board[enemy_pos[0]][enemy_pos[1]] = 0
                    # Reset the en_passant
                    self.en_passant = None
        else:
            # If we are not a pawn the possibility for en_passant closes
            self.en_passant = None

        # Check Rook moves
        if piece == 2:
            if y1 == 0:  # Queens side
                self.white_castle[0] = False
            elif y1 == 7:  # Kings side
                self.white_castle[1] = False
        elif piece == -2:
            if y1 == 0:  # Queens side
                self.black_castle[0] = False
            elif y1 == 7:  # Kings side
                self.black_castle[1] = False

        # Check King moves
        if piece == 5:
            # Check for white castling moves
            if self.white_castle[0] and y2 == 1:
                # Move the Rook
                self.board[7][0] = 0
                self.board[7][2] = 2
            elif self.white_castle[1] and y2 == 6:
                # Move the Rook
                self.board[7][7] = 0
                self.board[7][5] = 2
            # Set Castling to false
            self.white_castle = [False, False]

        # Check black king moves
        if piece == -5:
            # Check for black castling moves
            if self.black_castle[0] and y2 == 1:
                # Move the Rook
                self.board[0][0] = 0
                self.board[0][2] = -2
            elif self.black_castle[1] and y2 == 6:
                # Move the Rook
                self.board[0][7] = 0
                self.board[0][5] = -2
            # Set Castling to false
            self.black_castle = [False, False]

        self.board[x1][y1] = 0
        self.board[x2][y2] = piece

    def undo_move(self):
        """
        Undo the last move
        """
        self.board = self.board_state_stack.pop()
        self.black_castle = self.black_castle_stack.pop()
        self.white_castle = self.white_castle_stack.pop()

    # Function for finding the closest pieces given a direction
    def check_direction(self, pos, move_diagonal, move_straight):
        """
        Function that checks distance to the nearest piece or wall
        :param pos: [row, col]
        :param move_diagonal: Bool, True if the piece moves diagonally
        :param move_straight: Bool, True if the piece moves Straight
        :return: [Diagonal moves][Straight moves]
        """
        row, col = pos
        bot_half = self.board[row+1:]
        top_half = self.board[:row]

        diag_coll = []
        straight_coll = []
        if move_diagonal:
            # Check the top side of the board
            top_l = np.diagonal(top_half, col-row)  # Diagonal from the (row-col) eg. 1,2 = 1-2 = -1
            top_r = np.diagonal(np.fliplr(top_half), (7-col)-row)  # Diagonal from the opposite eg. 1, 2 = 7-1-2 = 4
            # Check the bottom half
            bot_l = np.diagonal(np.fliplr(bot_half), 7 - col + 1)
            bot_r = np.diagonal(bot_half, col + 1)

            # Check for the distance until a non-zero is found
            top_l = [0 if len(top_l) == 0 else min(7-row, col) if not np.any(top_l) else np.argmax(top_l[::-1] != 0) + 1]
            top_r = [0 if len(top_r) == 0 else min(row, 7-col) if not np.any(top_r) else np.argmax(top_r[::-1] != 0) + 1]
            bot_l = [0 if len(bot_l) == 0 else min(7-row, col) if not np.any(bot_l) else np.argmax(bot_l != 0) + 1]
            bot_r = [0 if len(bot_r) == 0 else min(7-row, 7-col) if not np.any(bot_r) else np.argmax(bot_r != 0) + 1]
            # Combine the results
            diag_coll = [top_r, top_l, bot_r, bot_l]
        if move_straight:
            # Define the vertical board
            right_half = self.board[:, col+1:]
            left_half = self.board[:, :col]

            # Check for nearest piece
            top = [0 if len(top_half) == 0 else row if not np.any(top_half[:, col]) else np.argmax(top_half[:, col][::-1] != 0) + 1]
            right = [0 if len(right_half[0]) == 0 else 7-col if not np.any(right_half[row]) else np.argmax(right_half[row] != 0) + 1]
            left = [0 if len(left_half[0]) == 0 else col if not np.any(left_half[row]) else np.argmax(left_half[row][::-1] != 0) + 1]
            bot = [0 if len(bot_half) == 0 else 7-row if not np.any(bot_half[:, col]) else np.argmax(bot_half[:, col] != 0) + 1]
            # Combine the results
            straight_coll = [top, right, left, bot]
        moves = [diag_coll, straight_coll]
        return moves

    def check_in_mate(self, start_pos, end_pos, side):
        """
        Check if the king is in mate after moving, by looking at the diagonal and straight moves

        :param start_pos: Start position of the piece moving = [row][col]
        :param end_pos: End position of the piece moving = [row][col]
        :param side: Side of the piece moving, 1 for white, -1 for black
        :return: True if movement results in a mate, False if not
        """
        # Move the board
        self.move_piece(start_pos, end_pos)

        # Find the kings position
        king_pos = np.where(self.board == side*5)

        # Check the straight line for Rooks and Queens
        hor = self.board[king_pos[0][0]]
        ver = self.board[:, king_pos[1][0]]
        # Non-zero vectors
        hor_zero = np.nonzero(hor)[0]  # Zero is due to numpy bullshittery
        ver_zero = np.nonzero(ver)[0]
        # Find the index on the non_zero vector
        hor_indices = np.where(hor_zero == king_pos[1][0])[0][0]
        ver_indices = np.where(ver_zero == king_pos[0][0])[0][0]

        # Check the top and left half
        if (hor_indices > 0 and hor[hor_zero[hor_indices - 1]] == -1*side*2 | -1*side*6) or \
                (ver_indices > 0 and ver[ver_zero[ver_indices - 1]] == -1*side*2 | -1*side*6):
            self.undo_move()
            return True
        # Check the bottom and right half
        if (hor_indices != np.shape(hor_zero)[0] - 1 and hor[hor_zero[hor_indices + 1]] == -1*side*2 | -1*side*6) or \
                (ver_indices != np.shape(ver_zero)[0] - 1 and ver[ver_zero[ver_indices + 1]] == -1*side*2 | -1*side*6):
            self.undo_move()
            return True

        # Check the diagonals for pawns, bishops and Queens
        diag_l = np.diagonal(self.board, king_pos[1][0] - king_pos[0][0])  # Diagonal from the (row-col) eg. 1,2 = 1-2 = -1
        diag_r = np.diagonal(np.fliplr(self.board), (7 - king_pos[1][0]) - king_pos[0][0])  # Diagonal from the opposite eg. 1, 2 = 7-1-2 = 4
        # Find the king index in each of the diagonals
        l_ind = min(king_pos[0][0], king_pos[1][0])  # min(row, distance to wall)
        r_ind = min(king_pos[0][0], 7-king_pos[1][0]) # min(row, distance to wall)
        # Check for pawns first
        if diag_l[l_ind+(-1*side)] == -1*side or diag_r[r_ind+(-1*side)] == -1*side:
            self.undo_move()
            return True
        # Check for bishops
        l0 = np.nonzero(diag_l)[0]  # Zero is due to numpy bullshittery
        r0 = np.nonzero(diag_r)[0]
        # Find the index on the non_zero vector
        left_indices = np.where(l0 == l_ind)[0][0]
        right_indices = np.where(r0 == r_ind)[0][0]

        # Check the top half
        if (left_indices > 0 and diag_l[l0[left_indices - 1]] == -1*side*4 | -1*side*6) or\
                (right_indices > 0 and diag_r[r0[right_indices - 1]] == -1*side*4 | -1*side*6):
            self.undo_move()
            return True
        # Check the bottom half
        if (left_indices != np.shape(l0)[0] - 1 and diag_l[l0[left_indices + 1]] == -1*side*4 | -1*side*6) or\
                (right_indices != np.shape(r0)[0] - 1 and diag_r[r0[right_indices + 1]] == -1*side*4 | -1*side*6):
            self.undo_move()
            return True

        self.undo_move()
        return False

    def get_valid_moves(self, position):
        """
        Function that returns valid moves for a piece
        :param position: [row][col]
        :return: list of valid moves, for given piece
        """
        y = position[0]
        x = position[1]
        piece = self.board[y][x]  # 1 = P, 2 = R, 3 = N, 4 = B, 5 = K, 6 = Q

        if piece == 0:
            return []

        # Simplify the piece representation
        side = 1 if piece >= 1 else -1  # 1 for white, -1 for black

        # If the king is missing the game is over, and we return with no moves
        if not np.any(self.board == side*5):
            return []

        piece = abs(piece)

        if piece == 1:  # pawn
            valid_moves = self.pawn_move_calc(position, int(-1*side))
        elif piece == 3:  # knight
            valid_moves = self.knight_move_calc(position, side)
        elif piece == 5:  # king
            valid_moves = self.king_move_calc(position, side)
        else:  # The rest
            # Set the movement type depending on the piece
            move_type = (True, False) if piece == 2 else (False, True) if piece == 4 else (True, True) # (Diagonal, Straight)
            valid_moves = self.general_move_calc(position, move_type, side)

        # Check if any of the moves result in a checkmate
        i = 0
        while i < len(valid_moves):
            if self.check_in_mate(valid_moves[i][0], valid_moves[i][1], side):
                # Remove the move if it's illegal
                valid_moves.pop(i)
            i += 1

        return valid_moves

    def general_move_calc(self, pos, move_type, side):
        """
        Function that retrieves possible moves for the rook, bishop or queen
        :param pos: [row][col]
        :param move_type: [Bool: Straight move][Bool: Diagonal move]
        :param side: 1 for white, -1 for black
        :return: list of possible moves
        """
        # Go through the possible moves, defined by the piece type
        d_diag, d_straight = self.check_direction(pos, move_type[1], move_type[0])
        v_out = []
        # Iterate over elements of moves and increment
        if move_type[0]:  # Check straight moves
            # set up a vector to store the moves in

            increments = [(-1, 0),  # up
                          (0, 1),  # right
                          (0, -1),  # left
                          (1, 0)]  # down

            # Iterate through each increment and delta, and update the current value accordingly
            for j, incr in enumerate(increments):
                incremented_pos = np.array(pos)
                delta = d_straight[j]
                if len(delta) == 0 or delta[0] == 0:
                    continue
                for _ in range(delta[0]):
                    incremented_pos = incremented_pos + np.array(incr)
                    possible_pos = [pos, incremented_pos]
                    # Append a copy of the current value to the results list
                    v_out.append(possible_pos)
                # If the last piece is an ally we remove it
                if self.has_ally(v_out[-1][1], side):
                    v_out.pop(-1)
        if move_type[1]:  # Check Diagonal moves
            # set up a vector to store the moves in
            increments = [(-1, 1),  # up-right
                          (-1, -1),  # up-left
                          (1, 1),  # down-right
                          (1, -1)]  # down-left

            # Iterate through each increment and delta, and update the current value accordingly
            for j, incr in enumerate(increments):
                incremented_pos = np.array(pos)
                delta = d_diag[j]
                if len(delta) == 0 or delta[0] == 0:
                    continue
                for _ in range(delta[0]):
                    incremented_pos = incremented_pos + np.array(incr)
                    possible_pos = [pos, incremented_pos]
                    # Append a copy of the current value to the results list
                    v_out.append(possible_pos)

                # If the last piece is an ally
                if self.has_ally(v_out[-1][1], side):
                    v_out.pop(-1)
        return v_out

    def pawn_move_calc(self, pos, direction):
        """
        Function that retrieves possible moves for the pawns
        :param pos: [row][col]
        :param direction: -1 for white, 1 for black
        :return: list of possible moves
        """
        v_out = []
        y, x = pos
        # Move straight
        # Check for blocking piece
        if y != 0 and y != 7:
            if self.board[y+1*direction][x] == 0:
                v_out.append([pos, (y+1*direction, x)])
                if ((direction == -1 and y == 6) and self.board[y+2*direction][x] == 0)\
                        or ((direction == 1 and y == 1) and self.board[y+2*direction][x] == 0):
                    v_out.append([pos, (y+2*direction, x)])

            # Check for diagonal movements
            if x+1 < 8 and self.has_enemy((y+1*direction, x+1), -1*direction):
                v_out.append([pos, (y+1*direction, x+1)])
            if x-1 >= 0 and self.has_enemy((y+1*direction, x-1), -1*direction):
                v_out.append([pos, (y+1*direction, x-1)])

        # Check for en passant
        start_pos = 1 if direction == 1 else 6
        if self.en_passant is not None and y != start_pos:
            en_passant_pos = np.array([[y+1*direction, x+1], [y+1*direction, x-1]])
            for current_pos in en_passant_pos:
                if current_pos[0] == self.en_passant[0] and current_pos[1] == self.en_passant[1]:
                    v_out.append(np.array([pos, current_pos]))

        return v_out

    def knight_move_calc(self, pos, side):
        """
        Function that retrieves possible moves for the Knight
        :param pos: [row][col]
        :param side: 1 for white, -1 for black
        :return: list of possible moves
        """
        # Since knights can only move in the L shape we check all those positions
        possible_moves = np.array([[-2, 1], [-1, 2], [1, 2], [2, 1], [2, -1], [1, -2], [-1, -2], [-2, -1]])
        original_pos = np.tile(np.array(pos), (len(possible_moves), 1))

        # Calculate the general moves
        general_moves = possible_moves+original_pos
        # Check if any of the moves leave the boundary
        boundary_mask = np.logical_and(general_moves >= 0, general_moves <= 7)
        boundary_mask = np.all(boundary_mask, axis=1)
        # Remove the ones that do leave the boundary
        possible_moves = general_moves[boundary_mask]

        # Check if there are allies in the positions
        row_indices = possible_moves[:, 0]
        col_indices = possible_moves[:, 1]

        # For white anything above zero is an ally, while for black it's below
        if side == 1:
            ally_mask = self.board[row_indices, col_indices] <= 0
        else:
            ally_mask = self.board[row_indices, col_indices] >= 0
        possible_moves = possible_moves[ally_mask]

        return list(np.stack([original_pos[:len(possible_moves)], possible_moves], axis=1))

    def king_move_calc(self, pos, side):

        """
        Function that retrieves possible moves for the king
        :param pos: [row][col]
        :param side: 1 for white, -1 for black
        :return: list of possible moves
        """
        possible_moves = np.array([[-1, 0],  # up
                                   [0, 1],  # right
                                   [0, -1],  # left
                                    [1, 0],
                                   [-1, 1],  # up-right
                                   [-1, -1],  # up-left
                                   [1, 1],  # down-right
                                   [1, -1]  # down-left
        ])
        original_pos = np.tile(np.array(pos), (len(possible_moves), 1))
        # Calculate the general moves
        general_moves = possible_moves + original_pos

        # Check if any of the moves leave the boundary
        boundary_mask = np.logical_and(general_moves >= 0, general_moves <= 7)
        boundary_mask = np.all(boundary_mask, axis=1)
        # Remove the ones that do leave the boundary
        possible_moves = general_moves[boundary_mask]

        # Check if there are allies in the positions
        row_indices = possible_moves[:, 0]
        col_indices = possible_moves[:, 1]

        # For white anything above zero is an ally, while for black it's below
        if side == 1:
            ally_mask = self.board[row_indices, col_indices] <= 0
        else:
            ally_mask = self.board[row_indices, col_indices] >= 0
        possible_moves = possible_moves[ally_mask]

        # Check for castling
        if side == 1 and np.any(self.white_castle) and pos[0] == 7:
            # Queens side castling
            if self.white_castle[0]:
                positions = np.array([[7, 1], [7, 2], [7, 3]])
                row_indices = positions[:, 0]
                col_indices = positions[:, 1]
                # Check if there are no pieces blocking the move
                if np.all((self.board[row_indices, col_indices] == 0)):
                    possible_moves = np.append(possible_moves, [[7, 1]], axis=0)
            # Kings side castling
            if self.white_castle[1]:
                positions = np.array([[7, 5], [7, 6]])
                row_indices = positions[:, 0]
                col_indices = positions[:, 1]
                # Check if there are no pieces blocking the move
                if np.all((self.board[row_indices, col_indices] == 0)):
                    # Add the move
                    possible_moves = np.append(possible_moves, [[7, 6]], axis=0)
        # Blacks side is the same
        elif side == -1 and np.any(self.black_castle) and pos[0] == 0:
            # Queens side castling
            if self.black_castle[0]:
                positions = np.array([[0, 1], [0, 2], [0, 3]])
                row_indices = positions[:, 0]
                col_indices = positions[:, 1]
                # Check if there are no pieces blocking the move
                if np.all((self.board[row_indices, col_indices] == 0)):
                    possible_moves = np.append(possible_moves, [[0, 1]], axis=0)
            # Kings side castling
            if self.black_castle[1]:
                positions = np.array([[0, 5], [0, 6]])
                row_indices = positions[:, 0]
                col_indices = positions[:, 1]
                # Check if there are no pieces blocking the move
                if np.all((self.board[row_indices, col_indices] == 0)):
                    # Add the move
                    possible_moves = np.append(possible_moves, [[0, 6]], axis=0)
        return list(np.stack([np.tile(np.array(pos), (len(possible_moves), 1)), possible_moves], axis=1))