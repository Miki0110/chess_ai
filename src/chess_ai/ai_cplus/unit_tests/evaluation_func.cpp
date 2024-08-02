#include <iostream>
#include <cassert>
#include "eval_functions.h"

void test_material_evaluation() {
    std::array<std::array<int, 8>, 8> board = {{
        {2, 3, 4, 6, 5, 4, 3, 2},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {-2, -3, -4, -6, -5, -4, -3, -2}
    }};
    int score = sum_material_values(board);
    assert(score == 0); // Balanced material
    std::cout << "Material Evaluation Test Passed!\n";
}

int main() {
    test_material_evaluation();
    std::cout << "All Evaluation Function Tests Passed!\n";
    return 0;
}
