from subprocess import Popen, PIPE
import os
import time


class CplusAI:
    def __init__(self):
        # Get the directory where the current file is located
        current_dir = os.path.dirname(os.path.abspath(__file__))
        # Get the path to the project folder
        project_dir = os.path.abspath(os.path.join(current_dir, '..'))

        # Get the folder path
        if project_dir.split('\\')[-1] == 'chess_ai':
            folder_path = os.path.join(project_dir, 'ai_cplus\\board_representation.exe')
        else:
            folder_path = os.path.join(project_dir, 'chess_ai\\ai_cplus\\board_representation.exe')
        # Launch the C++ program as a subprocess
        self.cpp_process = Popen(folder_path, shell=True, stdin=PIPE, stdout=PIPE)

    def cpp_minimax(self, message):
        # Add a new line for the c++ program
        message = message + '\n'
        # Send the string
        self.cpp_process.stdin.write(message.encode())
        self.cpp_process.stdin.flush()

        resulting_move = self.cpp_process.stdout.readline().strip()
        return resulting_move


if __name__ == "__main__":
    cpp = CplusAI()
    time.sleep(1)
    while True:
        string = 'HEY\n'
        result = cpp.send_string(string)
        print(result)