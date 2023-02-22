

class Move:
    def __init__(self, initial, final):
        self.initial = initial
        self.final = final
        self.castling = False
        self.en_passant = False

    # Defining when two objects are equal
    def __eq__(self, other):
        return self.initial == other.initial and self.final == other.final