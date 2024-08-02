import urllib.request
import os

# Get the directory where the current file is located
current_dir = os.path.dirname(os.path.abspath(__file__))
# Get the path to the project folder
project_dir = os.path.abspath(os.path.join(current_dir, '..'))
# Get the folder path to store the pgns in
folder_path = os.path.join(project_dir, 'data', 'pgn')

# Get the player we want data from
usernames = ["magnuscarlsen", "hikarunakamura", "bobbyfischer", "garrykasparov"]
for username in usernames:
    baseUrl = "https://api.chess.com/pub/player/" + username + "/games/"
    archivesUrl = baseUrl + "archives"

    # read the archives url and store in a list
    f = urllib.request.urlopen(archivesUrl)
    archives = f.read().decode("utf-8")
    archives = archives.replace("{\"archives\":[\"", "\",\"")
    archivesList = archives.split("\",\"" + baseUrl)
    archivesList[len(archivesList)-1] = archivesList[len(archivesList)-1].rstrip("\"]}")

    # download all the archives
    for i in range(len(archivesList)-1):
        url = baseUrl + archivesList[i+1] + "/pgn"
        filename = username + '-' + archivesList[i+1].replace("/", "-")
        urllib.request.urlretrieve(url, folder_path + "/" + filename + ".pgn")
        print(filename + ".pgn has been downloaded.")

print("All files have been downloaded.")