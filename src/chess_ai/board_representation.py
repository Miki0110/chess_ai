import numpy as np


class ChessBoard:
    def __init__(self, FEN_string):
        self.board = self.set_pieces(FEN_string)
        self.en_passant = []
        self.black_castle = [True, False]  # Queens-, Kings -side
        self.current_player = 1  # 1 = white, -1 = black

    def set_pieces(self, FEN):
        """
        Takes a FEN string and sets a numpy array up containing the board pieces from that
        """
        # rnbqkbnr/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQKBNR w KQ g4 1 1
        # Split the string into each of the information types
        fen_split = FEN.split('/')
        pieces = fen_split[:-1] + [fen_split[-1].split(' ')[0]]
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
        return board

    def print_board(self):
        print(self.board)

    def move_piece(self, start_pos, end_pos):
        x1, y1 = start_pos
        x2, y2 = end_pos
        piece = self.board[x1][y1]
        self.board[x1][y1] = 0
        self.board[x2][y2] = piece

    def get_valid_moves(self, position):
        x, y = position
        piece = self.board[x][y]  # 1 = P, 2 = R, 3 = N, 4 = B, 5 = K, 6 = Q
        valid_moves = np.array(1, dtype=np.uintc)

        if piece == 0:
            return valid_moves

        # Simplify the piece representaion
        side = 1 if piece == 1 else -1  # 1 for white, -1 for black
        piece = abs(piece)

        # Set the movement type depending on the piece
        move_type = [np.asarray([  # Rook
                    (-1, 0),  # up
                    (0, 1),  # right
                    (1, 0),  # down
                    (0, -1)  # left
        ]) if piece == 2 else
                    [np.asarray([  # Bishop and Pawn
                    (-1, 0),  # up
                    (0, 1),  # right
                    (1, 0),  # down
                    (0, -1) # left
        ]) if piece == 4 or piece == 1 else
                    np.asarray([  # Queen
                    (-1, 1),  # up-right
                    (-1, -1),  # up-left
                    (1, 1),  # down-right
                    (1, -1),  # down-left
                    (-1, 0),  # up
                    (0, 1),  # right
                    (1, 0),  # down
                    (0, -1)])  # left
        ]]

        if piece == 1:  # pawn
            self.pawn_move_calc(move_type)
        elif piece == 3:  # knight
            self.knight_move_calc(move_type)
        elif piece == 5:  # king
            self.king_move_calc(move_type)
        else:  # The rest
            self.general_move_calc(move_type)
        return valid_moves

    def general_move_calc(self, move_directions):
        pass

    def pawn_move_calc(self, move_directions):
        pass

    def knight_move_calc(self, move_directions):
        pass

    def king_move_calc(self, move_directions):
        pass