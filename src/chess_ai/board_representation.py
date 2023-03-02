import numpy as np


class ChessBoard:
    def __init__(self, FEN_string):
        self.en_passant = None
        self.black_castle = [True, True]  # Queens-, Kings -side
        self.white_castle = [True, True]  # Queens-, Kings -side
        self.current_player = 1  # 1 = white, -1 = black
        self.board = self.set_board(FEN_string)

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
        start_pos = np.array(start_pos)
        end_pos = np.array(end_pos)
        x1, y1 = np.array(start_pos)
        x2, y2 = np.array(end_pos)
        piece = self.board[x1][y1]

        # Check for pawns
        if abs(piece) == 1:
            # If we opened up for an en passant
            if abs(x1-x2) > 1:
                self.en_passant = end_pos+(-1*piece)  # This looks dumb but i needed the direction
            # Check if we did one instead
            if np.array_equal(end_pos, self.en_passant):
                # Get the place of the pawn
                enemy_pos = end_pos + piece
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

    # Function for finding the closest pieces given a direction
    def check_direction(self, pos, move_diagonal, move_straight):
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
            top_l = [0 if len(top_l) == 0 else col if not np.any(top_l) else np.argmax(top_l[::-1] != 0) + 1]
            top_r = [0 if len(top_r) == 0 else 7-col if not np.any(top_r) else np.argmax(top_r[::-1] != 0) + 1]
            bot_l = [0 if len(bot_l) == 0 else col if not np.any(bot_l) else np.argmax(bot_l != 0) + 1]
            bot_r = [0 if len(bot_r) == 0 else 7-col if not np.any(bot_r) else np.argmax(bot_r != 0) + 1]
            # Combine the results
            diag_coll = np.array((top_r, top_l, bot_r, bot_l), dtype=int)
        if move_straight:
            # Define the vertical board
            right_half = self.board[:, col+1:]
            left_half = self.board[:, :col]

            # Check for nearest piece
            top = [0 if len(top_half) == 0 else row if not np.any(top_half[:, col]) else np.argmax(top_half[:, col][::-1] != 0) + 1]
            right = [0 if len(right_half[0]) == 0 else 7-col if not np.any(right_half[row]) else np.argmax(right_half[row][::-1] != 0) + 1]
            left = [0 if len(left_half[0]) == 0 else col if not np.any(left_half[row]) else np.argmax(left_half[row] != 0) + 1]
            bot = [0 if len(bot_half) == 0 else 7-row if not np.any(bot_half[:, col]) else np.argmax(bot_half[:, col] != 0) + 1]
            # Combine the results
            straight_coll = np.array((top, right, left, bot), dtype=int)
        moves = [diag_coll, straight_coll]
        return moves

    def get_valid_moves(self, position):
        # TODO: Check for in check moves
        y = position[0]
        x = position[1]
        piece = self.board[y][x]  # 1 = P, 2 = R, 3 = N, 4 = B, 5 = K, 6 = Q

        if piece == 0:
            return []

        # Simplify the piece representation
        side = 1 if piece >= 1 else -1  # 1 for white, -1 for black
        piece = abs(piece)

        # Set the movement type depending on the piece
        # Move type = Diagonal, Straight
        move_type = (True, False) if piece == 2 else (False, True) if piece == 4 else (True, True)

        if piece == 1:  # pawn
            valid_moves = self.pawn_move_calc(position, int(-1*side))
        elif piece == 3:  # knight
            valid_moves = self.knight_move_calc(position, side)
        elif piece == 5:  # king
            valid_moves = self.king_move_calc(position, side)
        else:  # The rest
            valid_moves = self.general_move_calc(position, move_type, side)

        return valid_moves

    def general_move_calc(self, pos, move_type, side):
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
                possible_pos = np.array(pos)
                delta = d_straight[j]
                if len(delta) == 0 or delta[0] == 0:
                    continue
                for _ in range(delta[0]):
                    possible_pos = possible_pos + np.array(incr)
                    # Append a copy of the current value to the results list
                    v_out.append(possible_pos)
                else:
                    # If the last piece is an ally we remove it
                    if self.has_ally(v_out[-1], side):
                        v_out.pop(-1)
        if move_type[1]:  # Check Diagonal moves
            # set up a vector to store the moves in
            increments = [(-1, 1),  # up-right
                          (-1, -1),  # up-left
                          (1, 1),  # down-right
                          (1, -1)]  # down-left

            # Iterate through each increment and delta, and update the current value accordingly
            for j, incr in enumerate(increments):
                possible_pos = np.array(pos)
                delta = d_diag[j]
                if len(delta) == 0 or delta[0] == 0:
                    continue
                for _ in range(delta[0]):
                    possible_pos = possible_pos + np.array(incr)
                    # Append a copy of the current value to the results list
                    v_out.append(possible_pos)
                else:
                    # If the last piece is an ally
                    if self.has_ally(v_out[-1], side):
                        v_out.pop(-1)
        return np.array(v_out)

    def pawn_move_calc(self, pos, direction):
        v_out = []
        y, x = pos
        # Move straight
        if y != 0 or y != 7 and self.board[y+1*direction][x] == 0:
            v_out.append(np.array((y+1*direction, x)))
        if direction == -1 and y == 6 or direction == 1 and y == 1:
            v_out.append(np.array((y+2*direction, x)))

        # Check for diagonal movements
        if x+1 < 8 and self.has_enemy((y+1*direction, x+1), -1*direction):
            v_out.append(np.array((y+1*direction, x+1)))
        if x-1 >= 0 and self.has_enemy((y+1*direction, x-1), -1*direction):
            v_out.append(np.array((y+1*direction, x-1)))

        # Check for en passant
        if self.en_passant is not None:
            en_passant_pos = np.array([[y+1*direction, x+1], [y+1*direction, x-1]])
            for current_pos in en_passant_pos:
                if np.array_equal(current_pos, self.en_passant):
                    v_out.append(current_pos)

        return np.array(v_out)

    def knight_move_calc(self, pos, side):
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

        return possible_moves

    def king_move_calc(self, pos, side):
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
        return possible_moves
