import os
import json

# Define the paths

workspace_folder = os.path.abspath(os.path.join(os.path.dirname(__file__), 'src/chess_ai/ai_cplus'))
unit_tests_folder = os.path.join(workspace_folder, "unit_tests")
headers_folder = os.path.join(workspace_folder, "headers")
print(headers_folder)
print(unit_tests_folder)
print(workspace_folder)

# Find all test files in the unit_tests folder
test_files = [f for f in os.listdir(unit_tests_folder) if f.endswith('.cpp')]

tasks = {
    "version": "2.0.0",
    "tasks": []
}

# Main miniMax build task
tasks["tasks"].append({
    "label": "build miniMax",
    "type": "shell",
    "command": "g++",
    "args": [
        "-g",
        f"{workspace_folder}/miniMax.cpp",
        "-o",
        f"{workspace_folder}/miniMax.exe",
        "-I",
        headers_folder
    ],
    "group": {
        "kind": "build",
        "isDefault": True
    },
    "problemMatcher": ["$gcc"],
    "detail": "Compile miniMax.cpp with headers"
})

# Add a task for each test file
for test_file in test_files:
    test_name = os.path.splitext(test_file)[0]
    tasks["tasks"].append({
        "label": f"build {test_name}",
        "type": "shell",
        "command": "g++",
        "args": [
            "-g",
            f"{unit_tests_folder}/{test_file}",
            "-o",
            f"{test_name}.exe",
            "-I",
            headers_folder
        ],
        "group": {
            "kind": "build",
            "isDefault": False
        },
        "problemMatcher": ["$gcc"],
        "detail": f"Compile {test_file} with headers"
    })

# Write the tasks.json file
with open(".vscode/tasks.json", "w") as f:
    json.dump(tasks, f, indent=4)
