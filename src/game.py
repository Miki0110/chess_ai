import pygame

from constants import *
from chess_board import Board, Square
from mouse import Mouse


class Game:
    """
    Class that is used to render anything happening on the chess board
    """
    def __init__(self):
        self.next_player = 'white'
        self.hovered_sqr = None
        self.board = Board()
        self.mouse = Mouse()

    """Rendering methods"""
    # Render the background
    def show_bg(self, surface):
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

    # Render the pieces
    def show_pieces(self, surface):
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
                        img = pygame.transform.scale(img, (original_size[0]*0.7, original_size[1]*0.7))
                        # center it
                        img_center = col * SQSIZE + SQSIZE // 2, row * SQSIZE + SQSIZE // 2
                        piece.texture_rect = img.get_rect(center=img_center)

                        # render it into the game window
                        surface.blit(img, piece.texture_rect)

    def show_moves(self, surface):
        theme = self.config.theme

        if self.dragger.dragging:
            piece = self.dragger.piece

            # loop all valid moves
            for move in piece.moves:
                # color
                color = theme.moves.light if (move.final.row + move.final.col) % 2 == 0 else theme.moves.dark
                # rect
                rect = (move.final.col * SQSIZE, move.final.row * SQSIZE, SQSIZE, SQSIZE)
                # blit
                pygame.draw.rect(surface, color, rect)

    def show_last_move(self, surface):
        theme = self.config.theme

        if self.board.last_move:
            initial = self.board.last_move.initial
            final = self.board.last_move.final

            for pos in [initial, final]:
                # color
                color = theme.trace.light if (pos.row + pos.col) % 2 == 0 else theme.trace.dark
                # rect
                rect = (pos.col * SQSIZE, pos.row * SQSIZE, SQSIZE, SQSIZE)
                # blit
                pygame.draw.rect(surface, color, rect)

    def show_hover(self, surface):
        if self.hovered_sqr:
            # color
            color = (180, 180, 180)
            # rect
            rect = (self.hovered_sqr.col * SQSIZE, self.hovered_sqr.row * SQSIZE, SQSIZE, SQSIZE)
            # blit
            pygame.draw.rect(surface, color, rect, width=3)

    # other methods

    def next_turn(self):
        self.next_player = 'white' if self.next_player == 'black' else 'black'

    def set_hover(self, row, col):
        self.hovered_sqr = self.board.squares[row][col]

    def change_theme(self):
        self.config.change_theme()

    def play_sound(self, captured=False):
        if captured:
            self.config.capture_sound.play()
        else:
            self.config.move_sound.play()

    def reset(self):
        self.__init__()