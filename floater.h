#pragma once

#include "graphics.h"
#include "spritemap.h"

class Floater {
  public:

    Floater(int n, int x, int y);

    void update(unsigned int elapsed);

    bool done() const { return life_timer_ < 0; }
    void draw(Graphics& graphics) const;

  private:

    static constexpr int kFloaterDuration = 1000;
    static constexpr int kFloaterMoveRate = kFloaterDuration / 10;

    SpriteMap names_;
    int n_, x_, y_;
    int life_timer_, move_timer_;
};
