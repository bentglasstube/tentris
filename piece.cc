#include "piece.h"

void Piece::draw(Graphics& graphics, int x, int y) const {
  for (int iy = 0; iy < 4; ++iy) {
    for (int ix = 0; ix < 4; ++ix) {
      if (block(ix, iy)) {
        blocks_.draw(graphics, color(), x + ix * 8, y + iy * 8);
      }
    }
  }
}

uint16_t Piece::bits() const {
  switch (shape_) {
    case Shape::I:
      if (rotation_ == 0) return 0b0000111100000000;
      if (rotation_ == 1) return 0b0010001000100010;
      if (rotation_ == 2) return 0b0000000011110000;
      if (rotation_ == 3) return 0b0100010001000100;

    case Shape::O:
      return 0b0110011000000000;

    case Shape::T:
      if (rotation_ == 0) return 0b0100111000000000;
      if (rotation_ == 1) return 0b0100011001000000;
      if (rotation_ == 2) return 0b0000111001000000;
      if (rotation_ == 3) return 0b0100110001000000;

    case Shape::L:
      if (rotation_ == 0) return 0b0010111000000000;
      if (rotation_ == 1) return 0b0100010001100000;
      if (rotation_ == 2) return 0b0000111010000000;
      if (rotation_ == 3) return 0b1100010001000000;

    case Shape::J:
      if (rotation_ == 0) return 0b1000111000000000;
      if (rotation_ == 1) return 0b0110010001000000;
      if (rotation_ == 2) return 0b0000111000100000;
      if (rotation_ == 3) return 0b0100010011000000;

    case Shape::Z:
      if (rotation_ == 0) return 0b1100011000000000;
      if (rotation_ == 1) return 0b0010011001000000;
      if (rotation_ == 2) return 0b0000110001100000;
      if (rotation_ == 3) return 0b0100110010000000;

    case Shape::S:
      if (rotation_ == 0) return 0b0110110000000000;
      if (rotation_ == 1) return 0b0100011000100000;
      if (rotation_ == 2) return 0b0000011011000000;
      if (rotation_ == 3) return 0b1000110001000000;
  }

  return 0;
}

int Piece::color() const {
  switch (shape_) {
    case Shape::I: return 8;
    case Shape::O: return 14;
    case Shape::T: return 6;
    case Shape::L: return 3;
    case Shape::J: return 10;
    case Shape::Z: return 4;
    case Shape::S: return 11;
    default: return 1;
  }
}

bool Piece::block(int x, int y) const {
  if (x < 0 || x > 3) return false;
  if (y < 0 || y > 3) return false;

  return (bits() >> (15 - 4 * y - x)) & 1;
}
