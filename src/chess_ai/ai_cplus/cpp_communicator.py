from subprocess import Popen, PIPE
import os
import time
import shlex  # For handling shell commands

class CplusAI:
    def __init__(self):
        # Get the directory where the current file is located
        current_dir = os.path.dirname(os.path.abspath(__file__))
        # Get the path to the project folder
        project_dir = os.path.abspath(os.path.join(current_dir, '..'))

        # Get the folder path
        if project_dir.split('\\')[-1] == 'chess_ai':
            folder_path = os.path.join(project_dir, 'ai_cplus', 'miniMax.exe')
        else:
            folder_path = os.path.join(project_dir, 'chess_ai', 'ai_cplus', 'miniMax.exe')

        # Launch the C++ program as a subprocess without shell
        self.cpp_process = Popen([folder_path], stdin=PIPE, stdout=PIPE, stderr=PIPE)

    def cpp_minimax(self, FEN, player, depth=5):
        message = f'{1 if player == "white" else -1},{depth},{FEN}\n'
        # Send the string
        self.cpp_process.stdin.write(message.encode())
        self.cpp_process.stdin.flush()

        resulting_move = [0, 0, 0, 0]
        while True:
            line_returned = self.cpp_process.stdout.readline().strip().decode("utf-8")
            print(line_returned)
            if "We are done" in line_returned:
                break
            if "Best move: " in line_returned:
                numbers = line_returned.split(" ")[-1].split(',')
                for i, number in enumerate(numbers):
                    resulting_move[i] = eval(number)

        return resulting_move

if __name__ == "__main__":
    cpp = CplusAI()
    time.sleep(1)
    while True:
        FEN = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
        result = cpp.cpp_minimax(FEN, 'white')
        print(result)
