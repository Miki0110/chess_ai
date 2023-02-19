import pygame

from constants import *

class Mouse:
    def __init__(self):
        self.piece = None
        self.dragging = False
        self.mouseX = 0
        self.mouseY = 0
        self.initial_row = 0
        self.initial_col = 0

    # blit method
    def render_blit(self, surface):
        # retrieve the texture
        self.piece.set_texture()
        texture = self.piece.texture
        # load the image
        img = pygame.image.load(texture)
        # Find the center position
        img_center = (self.mouseX, self.mouseY)
        self.piece.texture_rect = img.get_rect(center=img_center)
        # render it
        surface.blit(img, self.piece.texture_rect)

    def update_mouse(self, pos):
        self.mouseX, self.mouseY = pos

    def save_initial(self, pos):
        # Save the position on the board instead of pixel values
        self.initial_row = pos[1]
        self.initial_col = pos[0]

    def drag_piece(self, piece):
        self.piece = piece
        self.dragging = True

    def undrag_piece(self):
        self.piece = None
        self.dragging = False

    # Function for converting between pixel and board position
    def pix_to_pos(self, pos):
       board_pos = pos[0] // SQSIZE, pos[1] // SQSIZE
       return board_pos
