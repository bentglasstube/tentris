#include "floater.h"

Floater::Floater(int n, int x, int y) :
  names_("bonus.png", 1, 44, 8),
  n_(n), x_(x), y_(y),
  life_timer_(kFloaterDuration), move_timer_(kFloaterMoveRate)
{}

void Floater::update(unsigned int elapsed) {
  life_timer_ -= elapsed;
  move_timer_ -= elapsed;

  while (move_timer_ < 0) {
    move_timer_ += kFloaterMoveRate;
    --y_;
  }
}

void Floater::draw(Graphics& graphics) const {
  names_.draw(graphics, n_, x_, y_);
}
