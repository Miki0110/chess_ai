
import sys

from src.constants import *
from src.game_engine.chess_board import Square
from src.game_engine.game import Game
from src.game_engine.move import Move
from src.chess_ai.search_algorithm import *
from src.chess_ai.board_representation import *


AI_PLAY = True
DEBUG = True


# Idk why I made this into a class, could have just used global variables
class Main:
    def __init__(self):
        import pygame
        pygame.init()
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        pygame.display.set_caption('Chess display')
        self.game = Game()

    def main_loop(self):
        import pygame
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

            if AI_PLAY and game.curr_player == 'black':
                FEN = board.to_fen(game.curr_player)
                chess_ai = ChessBoard(FEN)
                #chess_ai.print_board()
                player = True if game.curr_player == 'white' else False
                #score, move = minimax(4, chess_ai, -float('inf'), float('inf'), player)
                score, move = minimax_multiprocess(3, chess_ai, processes=6, maximizing_player=player)
                print('current move: ', move)
                p = board.squares[move[0][0]][move[0][1]].piece
                print('piece at place: ', p.name)
                board.calc_moves(p, move[0][0], move[0][1])
                # Create the move
                initial = Square(move[0][0], move[0][1])
                final = Square(move[1][0], move[1][1])
                move_class = Move(initial, final)

                if board.valid_move(p, move_class):
                    board.move(p, move_class)
                    # Set the next players turn
                    game.next_turn()

            # Check for input
            for event in pygame.event.get():
                # click
                if event.type == pygame.KEYDOWN:
                    # DEBUG
                    if event.key == pygame.K_d:
                        player = 1 if game.curr_player == 'white' else -1
                        # Generate board
                        FEN = board.to_fen('white')
                        print(FEN)
                        chess_ai = ChessBoard(FEN)
                        chess_ai.print_board()
                        # Generate possible moves
                        moves = move_generator(chess_ai, player)
                        print(len(moves))
                        # Evaluate board rating
                        score = evaluate_board(chess_ai, player)
                        print(score)

                if event.type == pygame.MOUSEBUTTONDOWN:
                    mouse.update_mouse(event.pos)
                    clicked_pos = mouse.pix_to_pos((mouse.mouseX, mouse.mouseY))
                    # Check if the clicked position has a piece
                    if board.squares[clicked_pos[1]][clicked_pos[0]].has_piece():
                        # Retrieve the piece type
                        piece = board.squares[clicked_pos[1]][clicked_pos[0]].piece
                        if piece.color != game.curr_player:
                            break
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
                        # Set the next players turn
                        game.next_turn()

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