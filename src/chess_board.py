import copy

from constants import *
from pieces import *


class Board:
    def __init__(self):
        self.squares = [[Square(row, col) for col in range(COLS)] for row in range(ROWS)]
        self.last_move = None
        self._create()
        self._add_pieces('white')
        self._add_pieces('black')

    # Function for calculating the valid moves
    def calc_moves(self, piece, row, col):
        piece.calc_moves(self.squares, row, col)

    def move(self, piece, move):
        initial = move.initial
        final = move.final

        # update board
        self.squares[initial.row][initial.col].piece = None
        self.squares[final.row][final.col].piece = piece

        # Check for pawn promotion
        if isinstance(piece, Pawn):
            # Since pawns can't move backwards we can just assume it doesn't happen
            if final.row == 0 or final.row == 7:
                self.squares[final.row][final.col].piece = Queen(piece.color)

        if any(move.castling for move in piece.moves) and piece.moves[piece.moves.index(move)].castling:
            # Set the direction
            r_new_col = 2 if final.col == 1 else 5
            r_old_col = 0 if final.col == 1 else 7
            # update the board
            self.squares[final.row][r_new_col].piece = Rook(piece.color)
            self.squares[final.row][r_old_col].piece = None

        # note that the piece has moved
        piece.moved = True

        # Clear moves in the piece
        piece.clear_moves()

        # Save the move for later
        self.last_move = move

    # Simple check for valid move
    def valid_move(self, piece, move):
        # Else return true false
        if move not in piece.moves:
            return
        # Check if it results in a mate
        if not self.in_check(piece, move):
            return True

    # Function for invalidating moves that kill you
    def in_check(self, piece, move):

        # Copy our board and piece to check if anything happens after moving
        temp_piece = copy.deepcopy(piece)
        temp_board = copy.deepcopy(self)
        # Move the piece
        temp_board.move(temp_piece, move)

        # Find the current player's king
        king = None
        for row in range(ROWS):
            for col in range(COLS):
                square = temp_board.squares[row][col]
                if square.has_piece() and isinstance(square.piece, King) and square.piece.color == piece.color:
                    king = square
                    break
            if king is not None:
                break

        if king is None:
            raise ValueError("King not found")

        # Check if any enemy pieces can attack the king
        for row in range(ROWS):
            for col in range(COLS):
                square = temp_board.squares[row][col]
                if square.has_enemy_piece(piece.color):
                    # Check if the piece can attack the king
                    square.piece.calc_moves(temp_board.squares, row, col)
                    if square.piece.can_attack(temp_board.squares, row, col, king.row, king.col):
                        return True

        return False


    def _create(self):
        for row in range(ROWS):
            for col in range(COLS):
                self.squares[row][col] = Square(row, col)

    def _add_pieces(self, color):
        # Set starting point depending on which side we are
        row_pawn, row_other = (6, 7) if color == 'white' else (1, 0)

        # pawns
        for col in range(COLS):
            self.squares[row_pawn][col] = Square(row_pawn, col, Pawn(color))

        # knights
        self.squares[row_other][1] = Square(row_other, 1, Knight(color))
        self.squares[row_other][6] = Square(row_other, 6, Knight(color))

        # bishops
        self.squares[row_other][2] = Square(row_other, 2, Bishop(color))
        self.squares[row_other][5] = Square(row_other, 5, Bishop(color))

        # rooks
        self.squares[row_other][0] = Square(row_other, 0, Rook(color))
        self.squares[row_other][7] = Square(row_other, 7, Rook(color))

        # queen
        self.squares[row_other][3] = Square(row_other, 3, Queen(color))

        # king
        self.squares[row_other][4] = Square(row_other, 4, King(color))


class Square:
    """
    Class that defines what happens at a square
    """
    ALPHACOLS = {0: 'a', 1: 'b', 2: 'c', 3: 'd', 4: 'e', 5: 'f', 6: 'g', 7: 'h'}

    def __init__(self, row, col, piece=None):
        self.row = row
        self.col = col
        self.piece = piece
        self.alphacol = self.ALPHACOLS[col]

    def __eq__(self, other):
        return self.row == other.row and self.col == other.col

    def has_piece(self):
        return self.piece is not None

    def isempty(self):
        return not self.has_piece()

    def has_team_piece(self, color):
        return self.has_piece() and self.piece.color == color

    def has_enemy_piece(self, color):
        return self.has_piece() and self.piece.color != color

    def isempty_or_enemy(self, color):
        return self.isempty() or self.has_enemy_piece(color)

    @staticmethod
    def on_board(*args):
        for arg in args:
            if arg < 0 or arg > 7:
                return False
        return True

    @staticmethod
    def get_alphacol(col):
        ALPHACOLS = {0: 'a', 1: 'b', 2: 'c', 3: 'd', 4: 'e', 5: 'f', 6: 'g', 7: 'h'}
        return ALPHACOLS[col]