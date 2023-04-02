import chess.pgn
import os


def import_pgn(pgn_file_paths=None):
    if pgn_file_paths is None:
        # Get the directory where the current file is located
        current_dir = os.path.dirname(os.path.abspath(__file__))
        # Get the path to the project folder
        project_dir = os.path.abspath(os.path.join(current_dir, '..'))
        # Get the folder path
        folder_path = os.path.join(project_dir, 'data')
        # Create a list of files
        pgn_file_paths = [os.path.join(folder_path, game) for game in os.listdir(folder_path)]

    # Create a list for all the games
    games = []
    # Go through the pgn files
    for item in pgn_file_paths:
        # Load the current game in
        pgn = open(item, encoding="utf-8")
        # Continue through the file until there are no more games
        while True:
            moves = []
            game = chess.pgn.read_game(pgn)
            if game is None:
                break
            # Save the moves into the games list
            for move in game.mainline_moves():
                moves.append(move)
            # Yield the moves for later use
            yield moves


if __name__ == "__main__":
    games = import_pgn()
    print(games)