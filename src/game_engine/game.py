from src.constants import *
from src.game_engine.chess_board import Board, Square
from src.game_engine.mouse import Mouse


class Game:
    """
    Class that is used to render anything happening on the chess board
    """
    def __init__(self):
        import pygame
        self.curr_player = 'white'
        self.hovered_sqr = None
        self.board = Board()
        self.mouse = Mouse()


    """Rendering methods"""
    # Render the background
    def show_bg(self, surface):
        import pygame
        light_color = (234, 235, 200)
        dark_color = (119, 154, 88)
        for row in range(ROWS):
            for col in range(COLS):
                # Set the colors to a light and dark color, depending on if we are on equal or nonequal squares
                color = light_color if (row + col) % 2 == 0 else dark_color
                # rect
                rect = (col * SQSIZE, row * SQSIZE, SQSIZE, SQSIZE)
                # blit
                pygame.draw.rect(surface, color, rect)

    # Render coordinates
    def show_coord(self, surface):
        import pygame
        light_color = (234, 235, 200)
        dark_color = (119, 154, 88)
        # For drawing out the square coordinate(numbers)
        for row in range(ROWS):
            col = 0
            color = dark_color if (row + col) % 2 == 0 else light_color
            # define the label type
            label_font = pygame.font.SysFont('monospace', 18, bold=True)
            label = label_font.render(str(ROWS - row), 1, color)
            label_pos = (5, 5 + row * SQSIZE)
            # Render it
            surface.blit(label, label_pos)

        # For drawing out the square coordinate(letters)
        for col in range(COLS):
            row = 7
            color = dark_color if (row + col) % 2 == 0 else light_color
            # define the label type
            label_font = pygame.font.SysFont('monospace', 18, bold=True)
            label = label_font.render(Square.get_alphacol(col), 1, color)
            label_pos = col * SQSIZE + SQSIZE - 20, HEIGHT - 20
            # Render it
            surface.blit(label, label_pos)

    # Render the pieces
    def show_pieces(self, surface):
        import pygame
        # Loop through the board position and render the pieces if they are there
        for row in range(ROWS):
            for col in range(COLS):
                # Check if there is a piece
                if self.board.squares[row][col].has_piece():
                    piece = self.board.squares[row][col].piece

                    # render the pieces
                    if piece is not self.mouse.piece:
                        piece.set_texture()
                        # Load the image into pygame
                        img = pygame.image.load(piece.texture)
                        # Transform the image to fit the board size
                        original_size = img.get_size()
                        img = pygame.transform.scale(img, (original_size[0]*0.6, original_size[1]*0.6))
                        # center it
                        img_center = col * SQSIZE + SQSIZE // 2, row * SQSIZE + SQSIZE // 2
                        piece.texture_rect = img.get_rect(center=img_center)

                        # render it into the game window
                        surface.blit(img, piece.texture_rect)

    def show_moves(self, surface):
        import pygame
        light_color = '#C86464'
        dark_color = '#C84646'
        if self.mouse.dragging:
            piece = self.mouse.piece

            # loop all valid moves
            for move in piece.moves:
                # color
                color = light_color if (move.final.row + move.final.col) % 2 == 0 else dark_color
                # rect
                rect = (move.final.col * SQSIZE, move.final.row * SQSIZE, SQSIZE, SQSIZE)
                # blit
                pygame.draw.rect(surface, color, rect)

    def show_last_move(self, surface):
        import pygame
        light_color = (244, 247, 116)
        dark_color = (172, 195, 51)
        if self.board.last_move:
            initial = self.board.last_move.initial
            final = self.board.last_move.final

            for pos in [initial, final]:
                # color
                color = light_color if (pos.row + pos.col) % 2 == 0 else dark_color
                # rect
                rect = (pos.col * SQSIZE, pos.row * SQSIZE, SQSIZE, SQSIZE)
                # blit
                pygame.draw.rect(surface, color, rect)

    # other methods

    def next_turn(self):
        self.curr_player = 'white' if self.curr_player == 'black' else 'black'

    def reset(self):
        self.__init__()