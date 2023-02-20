import pygame
import sys

from constants import *
from chess_board import Square, Board
from game import Game
from move import Move


# Idk why I made this into a class, could have just used global variables
class Main:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        pygame.display.set_caption('Chess display')
        self.game = Game()

    def main_loop(self):

        screen = self.screen
        game = self.game
        board = self.game.board
        mouse = self.game.mouse

        while True:
            # Render the game
            game.show_bg(screen)
            game.show_last_move(screen)
            game.show_coord(screen)
            # Render the pieces
            game.show_pieces(screen)

            if mouse.dragging:  # If we are holding a piece
                # Render held piece and it's moves
                game.show_moves(screen)
                game.show_pieces(screen)
                mouse.render_blit(screen)

            # Check for input
            for event in pygame.event.get():
                # click
                if event.type == pygame.MOUSEBUTTONDOWN:
                    mouse.update_mouse(event.pos)
                    clicked_pos = mouse.pix_to_pos((mouse.mouseX, mouse.mouseY))
                    # Check if the clicked position has a piece
                    if board.squares[clicked_pos[1]][clicked_pos[0]].has_piece():
                        # Retrieve the piece type
                        piece = board.squares[clicked_pos[1]][clicked_pos[0]].piece
                        # Calc moves
                        board.calc_moves(piece, clicked_pos[1], clicked_pos[0])
                        # Save the values
                        mouse.save_initial(clicked_pos)
                        mouse.drag_piece(piece)

                # mouse motion
                elif event.type == pygame.MOUSEMOTION:
                    # If we are holding a piece just update and render the position
                    if mouse.dragging:
                        mouse.update_mouse(event.pos)

                # click release
                elif event.type == pygame.MOUSEBUTTONUP:
                    if not mouse.dragging:
                        break
                    mouse.update_mouse(event.pos)
                    release_pos = mouse.pix_to_pos((mouse.mouseX, mouse.mouseY))

                    # Create the move
                    initial = Square(mouse.initial_row, mouse.initial_col)
                    final = Square(release_pos[1], release_pos[0])
                    move = Move(initial, final)

                    # Check if it's a valid move
                    if board.valid_move(mouse.piece, move):
                        # move the piece
                        board.move(mouse.piece, move)
                        game.show_pieces(screen)

                    mouse.undrag_piece()  # Let go of whatever we are holding

                # key press
                elif event.type == pygame.KEYDOWN:
                    pass

                # quit application
                elif event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()

            pygame.display.update()


if __name__ == "__main__":
    main = Main()
    main.main_loop()