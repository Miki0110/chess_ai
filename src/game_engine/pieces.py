import sys
import os
# Add src to the system path for module imports
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from game_engine.move import Move


class Piece:
    def __init__(self, name, color, value, texture=None, texture_rect=None):
        self.name = name
        self.color = color

        # This is for telling the computer which is good for it and which is not
        self.value = value

        self.moves = []
        self.moved = False
        self.move_directions = []

        # Set texture
        self.texture = texture
        self.set_texture()
        self.texture_rect = texture_rect

    def set_texture(self):
        self.texture = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..',
            f'images/{self.color[0]}_{self.name}_png_128px.png'))

    def add_move(self, move):
        self.moves.append(move)

    def clear_moves(self):
        self.moves = []

    def _create_move(self, in_row, in_col, end_row, end_col):
        from game_engine.chess_board import Square
        # Create squares
        initial = Square(in_row, in_col)
        final = Square(end_row, end_col)

        # Create the move
        move = Move(initial, final)
        return move

    # General function for checking if any piece can attack a specific position
    def can_attack(self, squares, start_row, start_col, t_row, t_col):
        from game_engine.chess_board import Square
        # Calculate possible moves
        self.calc_moves(squares, start_row, start_col)
        # Create move
        initial = Square(start_row, start_col)
        final = Square(t_row, t_col)
        # Create the move
        move = Move(initial, final)
        # Return if true
        return move in self.moves

    def calc_moves(self, squares, row, col):
        from game_engine.chess_board import Square
        # Go through the possible moves, defined by the piece type
        for row_incr, col_incr in self.move_directions:
            pos_move_row = row + row_incr
            pos_move_col = col + col_incr

            # Loop until something breaks the path
            while True:
                # If we leave bounds, or there's an ally, we break
                if not Square.on_board(pos_move_row, pos_move_col):
                    break
                if squares[pos_move_row][pos_move_col].has_team_piece(self.color):
                    break

                # Create move
                move = self._create_move(row, col, pos_move_row, pos_move_col)

                # check if the move is empty or has an enemy
                if squares[pos_move_row][pos_move_col].isempty():
                    # Append the move
                    self.add_move(move)
                # If it's an enemy
                elif squares[pos_move_row][pos_move_col].has_enemy_piece(self.color):
                    self.add_move(move)
                    break

                # Increment the move
                pos_move_row, pos_move_col = pos_move_row + row_incr, pos_move_col + col_incr

    # Function for retrieving the value of a piece, given the color of player
    def ret_value(self, player_color):
        value_sign = 1 if player_color == self.color else -1
        return self.value * value_sign


class Pawn(Piece):
    def __init__(self, color):
        # Since pawns can only move in one direction
        self.dir = -1 if color == 'white' else 1
        self.en_passant = False
        super().__init__('pawn', color, 1.0)

    def calc_moves(self, squares, row, col):
        from game_engine.chess_board import Square
        steps = 1 if self.moved else 2

        # Forward movement
        start = row + self.dir
        end = row + (self.dir * (1 + steps))
        for pos_move_row in range(start, end, self.dir):
            # If we are leaving the board
            if not Square.on_board(pos_move_row):
                break
            # If it's occupied
            if not squares[pos_move_row][col].isempty():
                break

            # create move
            move = self._create_move(row, col, pos_move_row, col)
            self.add_move(move)

        # Diagonal moves
        pos_move_row = row + self.dir
        pos_move_cols = [col-1, col+1]
        for pos_move_col in pos_move_cols:
            # Again can't leave the board
            if not Square.on_board(pos_move_row, pos_move_col):
                continue
            # If there's an enemy it's a possible move
            if squares[pos_move_row][pos_move_col].has_enemy_piece(self.color):
                # Create move
                move = self._create_move(row, col, pos_move_row, pos_move_col)
                self.add_move(move)

        en_pos = 3 if self.color == 'white' else 4
        # Check for en passant moves
        if en_pos != row:
            return
        pos_move_passant = [1] if col == 0 else ([-1] if col == 7 else [-1, 1])
        for direction in pos_move_passant:
            # Check if there's a possibility for en passant
            try:
                if squares[row][col+direction].has_enemy_piece(self.color) and squares[row][col+direction].piece.en_passant:
                    # Create the move
                    move = self._create_move(row, col, row+self.dir, col+direction)
                    move.en_passant = True
                    self.add_move(move)
            except Exception as e:
                print('failed en passant')



class Knight(Piece):
    def __init__(self, color):
        super().__init__('knight', color, 3.001)  # I like knights more <3

    def calc_moves(self, squares, row, col):
        from game_engine.chess_board import Square
        # Since knights can only move in the L shape we check all those positions
        possible_moves = [(-2, 1), (-1, 2), (1, 2), (2, 1), (2, -1), (1, -2), (-1, -2), (-2, -1)]
        possible_moves = [(row + x, col + y) for x, y in possible_moves]

        # Go through the possible moves
        for move in possible_moves:
            move_row, move_col = move
            if Square.on_board(move[0], move[1]):  # Checking if it is inside the board
                # Check occupation of square
                if squares[move_row][move_col].isempty_or_enemy(self.color):
                    # Create moves
                    move = self._create_move(row, col, move_row, move_col)
                    # append valid move
                    self.add_move(move)


class Bishop(Piece):
    def __init__(self, color):
        super().__init__('bishop', color, 3.0)
        self.move_directions = [
            (-1, 1),  # up-right
            (-1, -1),  # up-left
            (1, 1),  # down-right
            (1, -1)  # down-left
        ]


class Rook(Piece):
    def __init__(self, color):
        super().__init__('rook', color, 5.0)
        self.move_directions = [
            (-1, 0),  # up
            (0, 1),  # right
            (1, 0),  # down
            (0, -1)  # left
        ]


class Queen(Piece):
    def __init__(self, color):
        super().__init__('queen', color, 9.0)
        self.move_directions = [
            (-1, 1),  # up-right
            (-1, -1),  # up-left
            (1, 1),  # down-right
            (1, -1),  # down-left
            (-1, 0),  # up
            (0, 1),  # right
            (1, 0),  # down
            (0, -1)  # left
        ]


class King(Piece):
    def __init__(self, color):
        self.left_rook = None
        self.right_rook = None
        super().__init__('king', color, 10000.0)

    def calc_moves(self, squares, row, col):
        from game_engine.chess_board import Square
        # Go through the possible moves
        move_directions = [(-1, 1), (-1, -1), (1, 1), (1, -1), (-1, 0), (0, 1), (1, 0), (0, -1)]
        possible_moves = [(row + x, col + y) for x, y in move_directions]

        # Go through the possible moves
        for move in possible_moves:
            move_row, move_col = move
            if Square.on_board(move[0], move[1]):  # Checking if it is inside the board
                # Check occupation of square
                if squares[move_row][move_col].isempty_or_enemy(self.color):
                    # Create moves
                    move = self._create_move(row, col, move_row, move_col)
                    # append valid move
                    self.add_move(move)

        # Check for Castling
        if self.moved:  # Not possible if we've moved
            return
        # Check if left side
        empty = True
        for i in range(1, 4):
            # Check if there are any pieces blocking
            if squares[row][i].has_piece():
                empty = False
            # Check if there's a rook that hasn't moved
        if empty and isinstance(squares[row][0].piece, Rook) and not squares[row][0].piece.moved:
            # Create the move
            move = self._create_move(row, col, row, 2)
            # Set castling to true
            move.castling = True
            # append valid move
            self.add_move(move)

        empty = True
        # Check the right side
        for i in range(5, 7):
            # Check if there are any pieces blocking
            if squares[row][i].has_piece():
                empty = False
        # Check if there's a rook that hasn't moved
        if empty and isinstance(squares[row][7].piece, Rook) and not squares[row][7].piece.moved:
            # Create the squares for the move
            move = self._create_move(row, col, row, 6)
            # Set castling to true
            move.castling = True
            # append valid move
            self.add_move(move)