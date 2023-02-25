#include "Board.h"

std::vector<int> Space::GetIndices() {
    std::vector<int> indices = {
        bottom_left_index,
        bottom_left_index + 1,
        bottom_left_index + 9,
        bottom_left_index + 10,
    };
    return indices;
}

Board::Board() {
    // Build vertex positions
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            int base_pos = ((row * 9) + col) * 2;
            vertex_positions[base_pos] = -0.9 + ((float) row * (1.8/8));
            vertex_positions[base_pos + 1] = -0.9 + ((float) col * (1.8/8));
        }
    }

    // Build spaces
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int bottom_left_index = (row * 9) + col;
            SpaceColor color;
            if (bottom_left_index % 2 == 0) {
                color = SpaceColor::LIGHT;
            } else {
                color = SpaceColor::DARK;
            }
            spaces.push_back(Space(bottom_left_index, color));
        }
    }
}

void Board::ClearHighlights() {
    for (auto &space : spaces) {
        space.SetHighlight(false);
    }
}

int CursorCoordToCoord(double coord) {
    coord -= 300;
    coord /= 300;
    // Board's bounds
    if (coord < -0.9 || coord > 0.9) return -1;
    coord += 0.9;
    return coord / (1.8/8);
}

void Board::CheckHighlights(double x, double y) {
    // Window's bounds
    if (x < 0 || x > 600 || y < 0 || y > 600) return;
    y = 600-y;

    int row = CursorCoordToCoord(x);
    int col = CursorCoordToCoord(y);

    spaces[(row * 8) + col].SetHighlight(true);
}

void Board::ClearSelected() {
    for (auto &space : spaces) {
        space.SetSelected(false);
    }
}

void Board::ToggleSelected(double x, double y) {
    // Window's bounds
    if (x < 0 || x > 600 || y < 0 || y > 600) return;
    y = 600-y;

    int row = CursorCoordToCoord(x);
    int col = CursorCoordToCoord(y);

    auto &space = spaces[(row * 8) + col];
    ClearSelected();
    space.SetSelected(!space.IsSelected());
}