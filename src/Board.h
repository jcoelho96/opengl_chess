#ifndef BOARD_H
#define BOARD_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

enum class SpaceColor {
    DARK, LIGHT
};

class Space {
private:
    int bottom_left_index;

    SpaceColor space_color;

    bool highlighted = false;
    bool selected = false;

    unsigned int ibo;

public:
    Space(int bottom_left_index, SpaceColor color)
        : bottom_left_index(bottom_left_index)
        , space_color(color) {
            glGenBuffers(1, &ibo);
        }

    // Returns an array with the 4 indices of the vertices
    // that surround this space
    std::vector<int> GetIndices();
    
    SpaceColor GetSpaceColor() {return space_color;}

    bool IsHighlighted() {return highlighted;}
    bool IsSelected() {return selected;}

    void SetHighlight(bool highlight) {highlighted = highlight;}
    void SetSelected(bool select) {selected = select;}

    unsigned int GetIBO() {return ibo;}
};

class Board {
private:
    std::vector<Space> spaces;
    float vertex_positions[9*9*2];

public:
    Board();
    float *GetVertexPositions() {return vertex_positions;}
    std::vector<Space> &GetSpaces() {return spaces;}
    void ClearHighlights();
    void CheckHighlights(double x, double y);
    void ClearSelected();
    void ToggleSelected(double x, double y);
};

#endif