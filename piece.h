#pragma once

#include <vector>

#include "graphics.h"
#include "spritemap.h"

class Piece {
  public:

    enum class Shape { I, O, T, L, J, Z, S };

    Piece(int n) : Piece(static_cast<Shape>(n)) {}
    Piece(Piece::Shape shape) : blocks_("blocks.png", 19, 8, 8), shape_(shape), rotation_(0) {}
    void draw(Graphics& graphics, int x, int y, int texture = 0) const;

    void rotate_left() { rotation_ = (rotation_ + 3) % 4; }
    void rotate_right() { rotation_ = (rotation_ + 1) % 4; }

    Shape shape() const { return shape_; }
    bool block(int x, int y) const;
    int color() const;
    int rotation() const { return rotation_; }

  private:

    SpriteMap blocks_;
    Shape shape_;
    int rotation_;

    uint16_t bits() const;
};
